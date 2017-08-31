/****************************************************************************
**
**
**    Hex Wars - Copyright (C) 2017 Tomasz Ciborski
**
**    This file is part of Hex Wars.
**
**    Hex Wars is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    Hex Wars is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with Hex Wars.  If not, see <http://www.gnu.org/licenses/>.
**
**
****************************************************************************/
/****************************************************************************
 *
 *  /game/move.cpp
 *
 *  functions used for units behavior - pathfinding, movement performing attacks,
 *  capturing cities, spawning new units, destroying units, etc.
 *
 * *************************************************************************/

#include "../mainwind.h"

/* A* algorithm implementation. In order to hasten the calculations
 * I used approximation 42 pixels = 1 hex for calculating the distance,
 * the exact value is 24 * sqrt(3) = 1 hex ~~ 41.57 pixels */
QVector<int> MainWind::findPath(int start, int goal, u_int8_t mode){

    int current, neighbor, tentativeGScore;
    /* Let's clear all the data before we start */
    closedSet.clear();
    openSet.clear();
    cameFrom.clear();
    for(int i = 0; i < SIZEOF_MAP; i++){
        hexagon[i].fScore = SHRT_MAX;
        hexagon[i].gScore = SHRT_MAX;
    }
    PQinsert(openSet, start);
    hexagon[start].gScore = 0;
    /* Euclidean distance used for heuristic estimation */
    hexagon[start].fScore = sqrt(distanceBetweenHexes(start, goal))/42;

    while(!openSet.empty()){
        current = PQfront(openSet);
        if(current == goal)
            return reconstructPath(current);
        closedSet.append(current);
        foreach(neighbor, hexagon[current].contig){
            // Limitations on movement
            if(mode < 9 && !canAllocateNewUnit(neighbor, mode, curPlayer) &&
                    !(neighbor == goal && hasEnemyUnit(neighbor, curPlayer)
                      && mode != 1 && mode != 4)
                    && !(neighbor == goal && isShore(neighbor) && mode >= 4)) {
                continue;
            }
            /* if mode >= 9, we're doing 'emergency' movement - path, where
             * we're ignoring blocking units/cities */
            else if(mode >= 9 && !(mode == 9 && hexagon[neighbor].height > WATER_LEVEL
                                   && hexagon[neighbor].height <= NOACCESS_LEVEL)
                    && !(mode > 12 && hexagon[neighbor].height <= WATER_LEVEL)
                    && !(mode == 13 && hexagon[neighbor].island == hexagon[goal].island)
                    && !(mode >= 13 && neighbor == goal && isShore(neighbor)))
                continue;

            if(closedSet.indexOf(neighbor) != -1)
                continue;
            if(openSet.indexOf(neighbor) == -1)
                PQinsert(openSet, neighbor);
            tentativeGScore = hexagon[current].gScore + 1;
            if(tentativeGScore >= hexagon[neighbor].gScore) continue;

            cameFrom[neighbor] = current;
            hexagon[neighbor].gScore = tentativeGScore;
            hexagon[neighbor].fScore = hexagon[neighbor].gScore + sqrt(distanceBetweenHexes(neighbor, goal))/42;
        }
    }
    /* If the path doesn't exist, a plan B is to move the piece as close
     * to the goal as possible. We calculate the new path but this time
     * we assume that there are no blocking units */
    if(mode < 9) return findPath(start, goal, mode + 9);
    /* If even this kind of path doesn't exist, return empty path */
    else {
        QVector<int> error;
        return error;
    }
}

QVector<int> MainWind::reconstructPath(int current){
    QVector<int> totalPath;
    totalPath.append(current);
    while(cameFrom.constFind(current) != cameFrom.constEnd()){
        current = cameFrom[current];
        totalPath.append(current);
    }
    return totalPath;
}

/* Move the selected unit */
int MainWind::moveUnit(int start, int goal){
    int current, owner, type;
    owner = hexagon[start].unit[0].owner;
    type = hexagon[start].unit[0].type;
    movePath = findPath(start, goal, type);
    current = start;
    if(!movePath.empty()) movePath.pop_back();
    while(!movePath.empty() && hexagon[current].unit[0].moves){
        /* Attack enemy unit */
        if(hasEnemyUnit(movePath.back(), curPlayer)){
            if(type != 4){
                attackUnit(current, movePath.back());
                hexagon[current].unit[0].moves = 0;
                movePath.clear();
            }
            /* Transports can't attack! */
            else movePath.clear();
        }
        /* No enemy unit and we can move here*/
        else if(canAllocateNewUnit(movePath.back(), type, owner)){
            execSingleMove(current, movePath.back());
            current = movePath.back();
            movePath.pop_back();
            if(isHexVisible(current, lastHumanPlayer)){
                update();
                if(option[0]->isChecked()){
                    if(!type) Mix_PlayMusic(sound.mLand, 1);
                    else if(type <= BOMBER) Mix_PlayMusic(sound.mAir, 1);
                    else Mix_PlayMusic(sound.mNaval, 1);
                }
                delay();
            }
        }
        else movePath.clear();
    }
    return current;
}

/* Perform a single move between contiguous hexes */
void MainWind::execSingleMove(int from, int to){
    int playerMoved;
    hexagon[from].unit[0].moves--;
    playerMoved = hexagon[from].unit[0].owner;
    /* If we move a transport with tanks or aircraft carrier with planes,
     * we move the cargo first */
    if((hexagon[from].unit[0].type == TRANSPORT || hexagon[from].unit[0].type == CARRIER)
            && hexagon[from].unit.size() > 1){
        for(int i = 1; i < hexagon[from].unit.size(); i++)
            hexagon[to].unit.append(hexagon[from].unit[i]);
        hexagon[from].unit.resize(1);
    }


    hexagon[to].unit.push_front(hexagon[from].unit[0]);
    hexagon[from].unit.pop_front();
    unitAppear(to, !hexagon[to].unit[0].type ? 1 : 0, playerMoved);

    /* If 'to' is city, capture it */
    if(hexagon[to].terrain == 1 && city[hexagon[to].cityId].owner != playerMoved)
        captureCity(hexagon[to].cityId, playerMoved);

    /* If current player sees the action, we paint it onto the screen */
    if(lastHumanPlayer != playerMoved){
        if(isHexVisible(from, lastHumanPlayer)) paintHex(from);
        if(isHexVisible(to, lastHumanPlayer)) paintHex(to);
    }
    updateMinimap();
}

void MainWind::captureCity(int cityId, u_int8_t actor){
    if(city[cityId].owner != 6){
        player[city[cityId].owner].citiesControl--;
        unitDisappear(city[cityId].pos, 0, city[cityId].owner);
    }
    city[cityId].owner = actor;
    player[actor].citiesControl++;
    drawDockChart();
    for(int i = 0; i < SIZEOF_MAP; i++)
        drawMinimapHex(i);
    updateMinimap();
    if(actor == lastHumanPlayer){
        curCity = cityId;
        city[cityId].produces = 0;
        city[cityId].timeToProduce = rule.prodTime[0];
        cityWindow.prodOpt[0]->setChecked(true);
        cityWindow.setWindowIcon(QPixmap::fromImage(unitImage[54 + lastHumanPlayer]));
        doUpdateProdLabel();
        cityWindow.show();
    }
    else city[cityId].produces = NOPRODUCTION;
}

/* Tries to spawn a unit possibly as close to 'target' as possible */
bool MainWind::spawnUnit(int target, Unit toSpawn){
    QQueue<int> Q;
    int neighbor, u;
    for(int i = 0; i < SIZEOF_MAP; i++) hexagon[i].vis = 0;
    Q.append(target);
    hexagon[target].vis = 1;
    while(!Q.empty()){
        u = Q.front();
        Q.pop_front();
        if(canAllocateNewUnit(u, toSpawn.type, toSpawn.owner)){
            hexagon[u].unit.push_front(toSpawn);
            if(player[toSpawn.owner].isActive && !player[toSpawn.owner].isHuman){
                hexagon[u].unit[0].mode = ORDER_NOTHING;
                hexagon[u].unit[0].gotoHex = u;
                hexagon[u].unit[0].transpReq.clear();
                player[hexagon[u].unit[0].owner].unitsContType[hexagon[u].unit[0].type]++;
                player[hexagon[u].unit[0].owner].scoreProduced++;
            }
            player[toSpawn.owner].unitsControl++;
            unitAppear(u, toSpawn.type < 2 ? 1 : 0, toSpawn.owner);
            Q.clear();
            return true;
        }
        foreach(neighbor, hexagon[u].contig){
            if(!hexagon[neighbor].vis){
                Q.append(neighbor);
                hexagon[neighbor].vis = 1;
            }
        }
    }
    return false;
}

/* Check if the unit of chosen type can be allocated at the desired position */
bool MainWind::canAllocateNewUnit(int t, u_int8_t type, u_int8_t owner){
    if(hasEnemyUnit(t, owner) || !isHexUncovered(t, owner)) return false;
    if(type >= PARAS && type <= BOMBER && hexagon[t].terrain == 1 && !hasFriendlyCity(t, owner))
        return false;
    if(type == TANK && ((hexagon[t].terrain == 1 && hexagon[t].unit.size() < MAX_UNITS)
                || (hexagon[t].terrain != 1 && hexagon[t].height > WATER_LEVEL
                    && hexagon[t].height <= NOACCESS_LEVEL && hexagon[t].unit.empty())
                || (hasFriendlyShip(t, owner, TRANSPORT) && hexagon[t].unit.size() < MAX_NUNITS)))
        return true;
    else if(type == FIGHTER && ((hexagon[t].terrain == 1 && hexagon[t].unit.size() < MAX_UNITS)
                               || (hexagon[t].terrain != 1 && hexagon[t].unit.empty())
                               || (hasFriendlyShip(t, owner, CARRIER) && hexagon[t].unit.size()
                                   < MAX_NUNITS)))
        return true;
    else if((type == PARAS || type == BOMBER)
                && ((hexagon[t].terrain == 1 && hexagon[t].unit.size() < MAX_UNITS)
                    || (hexagon[t].terrain != 1 && hexagon[t].unit.empty())))
        return true;
    else if(type >= TRANSPORT && (hexagon[t].height <= WATER_LEVEL && hexagon[t].unit.empty()))
        return true;
    return false;
}

void MainWind::destroyUnit(int hex, u_int8_t actor){
    Unit toDestroy = hexagon[hex].unit[0];
    player[toDestroy.owner].unitsContType[toDestroy.type]--;
    /* If transport carries tanks, all of them are destroyed */
    if(toDestroy.type == TRANSPORT || toDestroy.type == CARRIER){
        player[toDestroy.owner].unitsControl -= hexagon[hex].unit.size();
        player[actor].scoreKilled += hexagon[hex].unit.size();
        if(toDestroy.type == TRANSPORT)
            player[toDestroy.owner].unitsContType[TANK] -= hexagon[hex].unit.size() - 1;
        else
            player[toDestroy.owner].unitsContType[FIGHTER] -= hexagon[hex].unit.size() -1;
        hexagon[hex].unit.clear();
    }
    else{
        hexagon[hex].unit.pop_front();
        player[toDestroy.owner].unitsControl--;
        player[actor].scoreKilled++;
    }
    if(isHexVisible(hex, lastHumanPlayer)){
        if(option[0]->isChecked())
            Mix_PlayMusic(sound.destroy, 1);
        /* Paint explosion on the hex */
        hexagon[hex].texture = paintUnit(&hexagon[hex].texture, &unitImage[71]);
        update();
        delay();
        paintHex(hex);
        update();
    }
    if(hexagon[hex].unit.empty() && curPlayer != toDestroy.owner)
        unitDisappear(hex, toDestroy.type < 2 ? 1 : 0, toDestroy.owner);
}

void MainWind::attackUnit(int from, int at){
    int a, d, i, dmg;
    a = hexagon[from].unit[0].type;
    d = hexagon[at].unit[0].type;
    dmg = attack.val[a][d];
    if(!dmg) return;
    dmg = (attack.val[a][d] == 255) ?
                255 : (dmg + R.rand() % dmg - dmg / 2);
    if(hexagon[at].unit[0].HP <= dmg){
        /* If we attack with bomber, destroy every piece in 'at' all hexes
         * contiguous to 'at'. */
        if(a == BOMBER){
            while(!hexagon[at].unit.empty())
                destroyUnit(at, curPlayer);
            foreach(i, hexagon[at].contig)
                if(i != from)
                    while(!hexagon[i].unit.empty())
                        destroyUnit(i, curPlayer);
        }
        else destroyUnit(at, curPlayer);
    }
    else{
        hexagon[at].unit[0].HP -= dmg;
        d = hexagon[at].unit[0].owner;
        if(d == lastHumanPlayer && isHexVisible(at, d)){
            paintHex(at);
            update();
        }
    }
    if(isHexVisible(at, lastHumanPlayer)
            && option[0]->isChecked() && sound.attack[a] != NULL)
        Mix_PlayMusic(sound.attack[a], 1);

}

Unit MainWind::newUnit(u_int8_t type, u_int8_t owner){
    Unit unit;
    unit.type = type;
    unit.owner = owner;
    unit.fuel = rule.maxFuel[type];
    unit.moves = rule.maxMove[type];
    unit.HP = 100;
    return unit;
}

/* Land paratroopers */
bool MainWind::canLandParas(int hex){
    if(hexagon[hex].height > WATER_LEVEL && hexagon[hex].height <= NOACCESS_LEVEL)
        return true;
    else return false;
}

void MainWind::landParas(int hex){
    hexagon[hex].unit[0].type = 0;
    hexagon[hex].unit[0].HP = 100;
    hexagon[hex].unit[0].moves = 0;
    if(isHexVisible(hex, lastHumanPlayer))
        paintHex(hex);
    player[hexagon[hex].unit[0].owner].unitsContType[PARAS]--;
    player[hexagon[hex].unit[0].owner].unitsContType[TANK]++;
}

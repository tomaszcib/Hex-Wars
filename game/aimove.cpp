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
 *  /game/aimove.cpp
 *
 *  main Artificial Intelligence routine. Includes movement and choice evaluation
 *  routines.
 *
 * *************************************************************************/

#include "../mainwind.h"

/* Main function handling computer move */
void MainWind::compMove(){
    u_int8_t vis;
    int allySize, enemySize, transSize, carrierSize,
         tmpSize, target, cityCount[9];
    /* Prepare following lists:
     *      - allyUnit: hexes containing curPlayer's units
     *      - enemyUnit: hexes containing hostile units and
     *                  hostile and neutral cities
     *      - transList: pointers to hexes in allyUnit which
     *                   contain transports
     *      - carryList: pointers to hexes in allyUnit which
     *                   contain carriers
     *      - cityCount: how many cities produce every type of unit */
    allySize = 0, enemySize = 0, transSize = 0, carrierSize = 0;
    for(int i = 0; i < 9; i++) cityCount[i] = 0;
    for(int i = 0; i < city.size(); i++)
        if(city[i].owner == curPlayer && city[i].produces < 9)
            cityCount[city[i].produces]++;

    for(int i = 0; i < SIZEOF_MAP; i++){
        hexagon[i].vis = HAS_NO_UNIT;
        if(!hexagon[i].unit.empty() && hexagon[i].unit[0].owner == curPlayer){
            hexagon[i].vis = HAS_ALLY_UNIT;
            allySize++;
            if(hasFriendlyShip(i, curPlayer, 4)) {
                hexagon[i].vis = HAS_ALLY_TRAN;
                transSize++;
            }
            else if(hasFriendlyShip(i, curPlayer, 7)){
                hexagon[i].vis = HAS_ALLY_CARR;
                carrierSize++;
            }
        }
        else if(hasEnemyUnit(i, curPlayer)){
            hexagon[i].vis = HAS_ENEMY_UNIT;
            enemySize++;
        }
    }
    /* Load the hexes into the lists */
    for(int i = 0, p1 = 0, p2 = 0; i < SIZEOF_MAP; i++){
        vis = hexagon[i].vis;
        if(vis >= HAS_ALLY_UNIT && vis < HAS_ENEMY_UNIT)
            allyUnit[p1++] = i;
        else if(vis == HAS_ENEMY_UNIT)
            enemyUnit[p2++] = i;
    }

    /* Handle the production first! */
    compProdAllCities(cityCount,allySize, enemySize, transSize);

    /* Shuffle allyUnit. The following code is used to generate a random
     * permutation of curPlayer's units. With this permutation, units are
     * picked on a non-fixed way, therefore such problems as handling
     * transport or assist requests are solved correctly, on average. */
    shuffleList(allyUnit, allySize);

    for(int i = 0, p1 = 0, p2 = 0; i < allySize; i++){
        if(hexagon[allyUnit[i]].vis == HAS_ALLY_TRAN)
            transList[p1++] = &allyUnit[i];
        else if(hexagon[allyUnit[i]].vis == HAS_ALLY_CARR)
            carrierList[p2++] = &allyUnit[i];
    }
    assessDanger( enemySize);

    /* Follow the routine. Move every unit exactly once, unless it's
     * a transport and we can perform multiple load/unload operations */
    for(int i = 0; i < allySize; i++){
        if(hexagon[allyUnit[i]].unit.empty()) continue;
        /* 1. If it's a transport */
        if(hexagon[allyUnit[i]].unit[0].type == TRANSPORT)
            //ai manage transport
            compManageTransport(i, allySize);
        /* 2. If we have at least one unit on this hex */
        else if(hexagon[allyUnit[i]].unit.size() >= 1){
            tmpSize = hexagon[allyUnit[i]].unit.size();
            while(tmpSize--){
                /* Check if there are units we can move on this hex.
                 * If not, process the next hex */
                if(!findFirstUndone(allyUnit[i])) break;
                /* Find a job for an unit */
                target = findJobCompSingleUnit(allyUnit[i], allySize,
                                               enemySize, transSize, carrierSize);
                /* If we've found a target, move an unit towards it */
                if(target != NO_TARGET)
                    target = moveUnit(allyUnit[i], target);
                /* If we're not moving a tank, pick a random destination to explore */
                else if(hexagon[allyUnit[i]].unit[0].type)
                    target = moveUnit(allyUnit[i], R.rand() % SIZEOF_MAP);
                /* If we're moving a tank, stay at position */
                else target = allyUnit[i];
                /* If we have moved paras unit here, check if we want and are able to drop them */
                if(hexagon[target].unit[0].type == PARAS){
                    if(APPROXDIST(target, hexagon[target].unit[0].gotoHex) < 4 && canLandParas(target))
                        landParas(target);
                }
                /* If we processed a unit, set its moves to 0 so that we won't end with an infinite loop */
                if(hexagon[target].unit[0].type != TANK
                        && (!hexagon[target].unit[0].transpReq.empty()
                            || hexagon[target].height <= WATER_LEVEL))
                    hexagon[target].unit[0].moves = 0;
                if(hasFriendlyShip(target, curPlayer, TRANSPORT))
                    putShipOnTop(target, TRANSPORT);
                else if(hasFriendlyShip(target, curPlayer, CARRIER))
                    putShipOnTop(target, CARRIER);
                if(!tmpSize) allyUnit[i] = target;
            }
        }
    }
}

/* Another big routine - task selector for a single AI unit */
int MainWind::findJobCompSingleUnit(int pos,int allySize, int enemySize, int transSize, int carrierSize){
    Unit *u, *u1;
    int maxAirRange = 0, closestFuelPoint = -1, target = -1, neighbor, neighbor2, want = 0, best = 0;
    u = &hexagon[pos].unit[0];
    /* 1. do some preparations for airborne units */
    if(u->type == FIGHTER || u->type == BOMBER){
        maxAirRange = (u->fuel - 1) * rule.maxMove[u->type] + u->moves;
        /* If we have less than half of maximum fuel, head to closest
         * replenishment point (either friendly city or carrier) */
        if(u->fuel <= rule.maxFuel[u->type] / 2){
            closestFuelPoint = findNearestFuelReplenish(pos, carrierSize);
            if(closestFuelPoint != -1 &&
                    APPROXDIST(pos, closestFuelPoint) < maxAirRange){
                target = closestFuelPoint;
                u->mode = ORDER_MOVE;
                return target;
            }
        }
    }
    /* If we're moving paras, pick best city that is within accepted range */
    else if(u->type == PARAS){
        /* If we don't have target yet, pick one */
        if(u->mode != ORDER_PARAS_ATTACK){
            target = parasMove(pos);

            if(target != NO_TARGET){
                u->mode = ORDER_PARAS_ATTACK;
                u->gotoHex = target;
            }
            else u->mode = ORDER_NOTHING;
        }
        /* If we already have a target, proceed to it and check where we can drop paras.
         * (as close as possible to the target city) */
        if(u->mode == ORDER_PARAS_ATTACK){
            foreach(neighbor, hexagon[u->gotoHex].contig){
                if(canAllocateNewUnit(neighbor, TANK, curPlayer)
                        && canLandParas(neighbor))
                    return neighbor;
            }
            foreach(neighbor, hexagon[u->gotoHex].contig)
                foreach(neighbor2, hexagon[neighbor].contig)
                    if(neighbor2 != u->gotoHex && canAllocateNewUnit(neighbor2, TANK, curPlayer)
                            && canLandParas(neighbor2))
                        return neighbor2;
            if(hexagon[pos].island == hexagon[u->gotoHex].island){
                landParas(pos);
                return NO_TARGET;
            }
        }
    }
    /* We handle fighter - a city defender */
    if(u->type == FIGHTER && hexagon[pos].terrain == 1){
        foreach(neighbor, hexagon[pos].contig){
            if(hasEnemyUnit(neighbor, curPlayer)){
                u1 = &hexagon[neighbor].unit[0];
                want = attack.val[FIGHTER][u1->type] + 100 - u1->HP;
                if(want > best){
                    best = want;
                    target = neighbor;
                }
            }
        }
        if(target != NO_TARGET){
            u->mode = ORDER_MOVE;
            return target;
        }
    }
    /* We're handling a tank that waits for a transport ride */
    if(u->type == TANK && u->mode == ORDER_WAIT_TRANSPORT){
        if(pos == u->transpReq.where){
            /* If a tank waited for over 5 turns and there's still no transport,
             * cancel the request */
            if(++u->transpReq.waitTime > 5){
                u->transpReq.clear();
                u->mode = ORDER_NOTHING;
            }
        }
    }
    /* Handle tank close-distance combat */
    if(u->type == TANK){
        /* Check if we can attack something on a contiguous hex */
        foreach(neighbor, hexagon[pos].contig){
            if(hasEnemyUnit(neighbor, curPlayer)){
                u1 = &hexagon[neighbor].unit[0];
                if(u1->type > TRANSPORT) continue;
                else if(u1->type == TANK)
                    want = 150 - u1->HP;
                else want = killDesire(rule.prodTime[u->type] * 2, attack.val[TANK][u1->type],
                                  rule.prodTime[TANK] * 2, attack.val[u1->type][TANK], 1);
                if(want > best){
                    best = want;
                    target = neighbor;
                }
            }
            /* If there's an enemy city on a contiguous hex, attack it */
            else if(hexagon[neighbor].terrain == 1
                    && city[hexagon[neighbor].cityId].owner != curPlayer){
                target = neighbor;
                break;
            }
        }
        if(target != NO_TARGET) {
            u->mode = ORDER_MOVE;
            return target;
        }
        /* If a tank is a city defender and the city is endangered,
         *  do not move the tank outside the city */
        else if(hexagon[pos].terrain == 1 && hexagon[pos].unit.size() == 1
                && cityData[hexagon[pos].cityId].urgency > 0){
            return pos;
        }
    }

    /* If previous conditions are not met, find something to kill for the unit */
    target = findSomethingToKill(pos, allySize, enemySize, transSize,
                                 (u->type >= PARAS && u->type <= BOMBER) ? 1 + maxAirRange / rule.maxMove[u->type]
                                 : 10);
    /* Explore the terrain */
    if(target == NO_TARGET) {
        u->mode = ORDER_EXPLORE;
        return NO_TARGET;
    }

    return target;
}

/* Algorithm of finding targets to conquer or units to destroy */
int MainWind::findSomethingToKill(int pos, int allySize,
                                  int enemySize, int transSize, int maxRange){
    Unit u, *u1;
    int moveTime, want = 0, vulnerability, index, index2 = -1,
            benefit = 0, tempHex = 0, min, tmp, tmp2,
            reserves, victimCount, targetShore = -1,
            bestTransport = -1,
            target = -1,
            best = 0;
    bool needsTransport = false, targetNeedsTransport = false, isTargetCity;
    isTargetCity = false;
    int attackValue;


    /* First check how every enemy city is endangered by our units */
    for(int i = 0; i < city.size(); i++){
        cityData[i].attack = 0;
        cityData[i].attackers = 0;
        cityData[i].occupy = 0;
    }
    for(int i = 0; i < allySize; i++){
        index = allyUnit[i];
        if(!hexagon[index].unit.empty())
            foreach(u, hexagon[index].unit)
                if(hexagon[u.gotoHex].terrain == 1 && city[hexagon[u.gotoHex].cityId].owner != curPlayer
                        && city[hexagon[u.gotoHex].cityId].owner != 6){
                    cityData[hexagon[u.gotoHex].cityId].attack += avgAttackVal(u.type, true);
                    cityData[hexagon[u.gotoHex].cityId].attackers++;
                    if(u.type == 0) cityData[hexagon[u.gotoHex].cityId].occupy++;
                }
    }

    /* Second, pick one target: iterate through all the cities,
     * and next, through all enemy units */

    u = hexagon[pos].unit[0];
    /* If we move a tank, find a closest coastal hex */
    if(u.type == TANK && transSize) tempHex = findShore(pos);

    // City iteration
    for(int i = 0; i < city.size(); i++){
        index = i;
        /* Ignore own cities */
        if(city[i].owner == curPlayer) continue;
        /* Ignore undiscovered cities - TODO.
         * Game needs proper AI explorations routines!*/
        //if(!isHexUncovered(city[i].pos, curPlayer)) continue;
        /* We can capture enemy city with tanks only. Without this
         * continue statement, we'd have ships blockading neutral cities */
        if(hexagon[city[i].pos].unit.empty() &&
                hexagon[pos].unit[0].type != TANK) continue;
        /* Under certain conditions, we may move a tank towards an enemy city */
        if(u.type == TANK && hexagon[pos].island != -1 &&
                hexagon[city[i].pos].island != hexagon[pos].island &&
                (scanCounts[hexagon[pos].island].enemyCities
                || scanCounts[hexagon[pos].island].neutCities ||
                !transSize || tempHex == -1)) continue;

        moveTime = APPROXDIST(pos, city[i].pos) / rule.maxMove[u.type];
        /* This means, we'll need a transport for a tank we're moving */
        if(u.type == TANK && (hexagon[pos].island != hexagon[city[i].pos].island)){
            bestTransport = -1;
            /* Distance from current position to the coast */
            moveTime = APPROXDIST(pos, tempHex);
            moveTime /= rule.maxMove[0];
            min = 32767;
            /* Check which transport is the best */
            for(int k = 0; k < transSize; k++){
                index2 = *transList[k];
                if(hasFriendlyShip(index2, curPlayer, 4)){
                    if(moveTime +
                            APPROXDIST(tempHex, index2) / rule.maxMove[TRANSPORT] > 5)
                        continue;
                    /* Transport already full, skip it */
                    if(hexagon[index2].unit.size() >= MAX_NUNITS) continue;
                    /* Calculate total distance transport->coast->target city */
                    tmp = APPROXDIST(tempHex, index2);
                    tmp2 = APPROXDIST(tempHex, city[i].pos);
                    tmp += tmp2;
                    /* We handle transports under construction. Add remaining building time
                     * to our temporary transport distance */
                    if(hexagon[index2].terrain == 1
                            && city[hexagon[index2].cityId].produces == TRANSPORT)
                        tmp += city[hexagon[index2].cityId].timeToProduce;
                    tmp /= rule.maxMove[4];
                    if(tmp < min) {
                        min = tmp;
                        bestTransport = index2;
                    }

                }
            }
            /* Select this transport */
            if(bestTransport != -1){
                moveTime += min;
                needsTransport = true;
            }
            /* We don't have any transport capable of carrying this unit. Skip the city. */
            else continue;
        }
        /* Check how enemy is vulnerable to attacks and how benefitial
         * would be killing that enemy */
        u = hexagon[pos].unit[0];
        vulnerability = 0;
        benefit = 0;
        if(!hexagon[city[i].pos].unit.empty()){
            for(int k = 0; k < hexagon[city[i].pos].unit.size(); k++)
                vulnerability += attack.val[hexagon[city[i].pos].unit[k].type][u.type];
            u1 = &hexagon[city[i].pos].unit[0];
            benefit = rule.prodTime[u1->type] * 2;
        }
        /* Check balance between hostile and allied units. If we have units
         * that are able to conquer the city once there are no defenders,
         * it would be great to occupy this city */
        reserves = cityData[i].attackers - hexagon[city[i].pos].unit.size();
        if(reserves > 0 && ((hexagon[pos].island == hexagon[city[i].pos].island)
                || cityData[i].occupy > 0))
            benefit += reserves / 5;
        /* Try calling allies as help */
        else if(reserves <= 0 && moveTime <= 10){
            for(int k = 0; k < allySize; k++){
                index2 = allyUnit[k];
                if(hexagon[index2].unit.empty()) continue;
                if(APPROXDIST(index2, city[i].pos) > maxRange)
                    continue;
                u1 = &hexagon[index2].unit[0];
                if(hasFriendlyShip(index2, curPlayer, 4))
                    tmp = APPROXDIST(index2, city[i].pos) / rule.maxMove[4];
                else if(hasFriendlyCity(index2, curPlayer)){
                    tmp = APPROXDIST(index2, city[i].pos) / rule.maxMove[0];
                }
                else tmp = APPROXDIST(index2, city[i].pos)
                        / rule.maxMove[u1->type];
                /* If the unit is close enough, mark it as possible ally */
                if(tmp <= 10){
                    if(u1->type == TRANSPORT)
                        cityData[i].attack += avgAttackVal(TANK, true) * hexagon[index2].unit.size() - 1;
                    else if(u1->type == CARRIER)
                        cityData[i].attack += avgAttackVal(FIGHTER, true) * hexagon[index2].unit.size() - 1;
                    else cityData[i].attack += (u1->type == PARAS ?
                                          avgAttackVal(TANK, true) : avgAttackVal(u1->type, true));
                }
            }
        }
        attackValue = avgAttackVal(u.type, true) + cityData[i].attack;
        victimCount = hexagon[city[i].pos].unit.size() + 1;

        /* City too far */
        if(moveTime > maxRange) want = 0;
        else if(u.type == DESTROYER || u.type == CARRIER){
            if(hexagon[city[i].pos].unit.size() &&
                    hexagon[city[i].pos].unit[0].type == TANK)
                want = 0;
        }
        /* No units able to conquer the city, we need them here */
        else if(hexagon[pos].island == hexagon[city[i].pos].island
                && cityData[i].attackers > 0 && !cityData[i].occupy
                && u.type == TANK)
            want = 20;
        /* Tank that is a city defender, may be moved outside the city
         * under the following circumstances */
        else if(u.type == TANK && hexagon[pos].island != -1 &&
                scanCounts[hexagon[pos].island].allyCities
                + scanCounts[hexagon[pos].island].neutCities
                >= scanCounts[hexagon[pos].island].enemyCities
                && hexagon[city[i].pos].island == hexagon[pos].island){
            want = 20 - moveTime;
        }
        else if(u.type == TANK && hexagon[city[i].pos].unit.size() < 2)
            want = 20 - moveTime + (city[i].owner == 6 ? (player[curPlayer].citiesControl < 6) * 10 : 0);
        /* Bombers use different evaluation than normal units */
        else if(u.type == BOMBER){
            want = bomberKillDesire(pos, city[i].pos) - moveTime;
        }
        else {
            want = killDesire(benefit, attackValue, rule.prodTime[u.type] * 2, vulnerability, victimCount);
            want -= moveTime;
        }

        /* Check if this target is currently the best */
        if(want > best){
            best = want;
            target = city[i].pos;
            isTargetCity = true;
            if(needsTransport){
                targetNeedsTransport = true;
                targetShore = tempHex;
            }
        }
    }
    // END OF CITY LOOOP

    // BEGIN UNIT LOOP
    u = hexagon[pos].unit[0];

    for(int i = 0; i < enemySize; i++){
        index = enemyUnit[i];
        /* Unit already destroyed */
        if(hexagon[index].unit.empty()) continue;
        /* Hex invisible - we can't see the unit. TODO */
        //if(!isHexUncovered(index, curPlayer)) continue;

        if(APPROXDIST(pos, index) / rule.maxMove[u.type] > maxRange)
            continue;

        /* We can't reach the unit, skip it */
        if(u.type == TANK && hexagon[index].island != hexagon[pos].island) continue;
        if(u.type > TRANSPORT && hexagon[index].height > WATER_LEVEL && !isShore(index))
            continue;
        if((u.type == TANK && (hexagon[index].height <= WATER_LEVEL
                               || hexagon[index].island != hexagon[pos].island))
                || (u.type > TRANSPORT && hexagon[index].height > WATER_LEVEL && !isShore(index)))
            continue;

        /* Evaluate the choice */
        u1 = &hexagon[index].unit[0];
        vulnerability = attack.val[u1->type][u.type];
        benefit = rule.prodTime[u1->type] * 2;
        moveTime = APPROXDIST(pos, index) / rule.maxMove[u.type];
        if(moveTime > maxRange) want = 0;
        else if(u.type == BOMBER) want = bomberKillDesire(pos, index);
        else if(attack.val[u.type][u1->type] <= 10 && u1->HP > 10) want = 0;
        else want = killDesire(benefit, attack.val[u.type][u1->type], rule.prodTime[u.type] * 2, vulnerability, 1);
        want -= moveTime;

        /* Check if it's currently the best one */
        if(want > best){
            isTargetCity = false;
            best = want;
            target = index;
            targetNeedsTransport = false;
        }
    }
    // END OF UNIT LOOP

    /* If we need transport, place a request for it */
    u1 = &hexagon[pos].unit[0];
    if(targetNeedsTransport){
        /* Mark that the unit is waiting for transport */      
        u1->mode = ORDER_WAIT_TRANSPORT;
        u1->transpReq.setTarget(targetShore, target, 0, best);
    }
    u1->gotoHex = target;
    return target;
}


/*********************************************************************************/

/* Calculate danger for every owned city. There are two kinds of danger:
 *      - urgency: power of enemy units that can attack our city within
 *                 3 turns,
 *      - grave danger: power of enemy units that can attack out city
 *                 within 1 turn.
 * Both values are substracted by strength of our defence */
void MainWind::assessDanger(int enemySize){
    Unit u;
    int urgency, dist, graveDanger, tmp;
    for(int i = 0; i < city.size(); i++){
        if(city[i].owner == curPlayer){
            urgency = 0, graveDanger = 0;
            for(int k = 0; k < enemySize; k++){
                foreach(u, hexagon[enemyUnit[k]].unit){
                    if(APPROXDIST(city[i].pos, enemyUnit[k])
                                < rule.maxMove[u.type] * 3){
                        /*dist = findPath(city[i].pos, enemyUnit[k], u.type + 9).size();
                        if(!dist) continue;
                        dist /= rule.maxMove[u.type];*/
                        dist = APPROXDIST(city[i].pos, enemyUnit[k]) / rule.maxMove[u.type];
                        if(dist <= 3){
                            tmp = avgAttackVal((u.type == PARAS) ? TANK : u.type, true) / (dist + 1);
                            urgency += tmp;
                            if(dist <= 1) graveDanger += tmp;
                        }
                    }
                }
            }
            foreach(u, hexagon[city[i].pos].unit){
                tmp = avgAttackVal(u.type, true);
                urgency -= tmp;
                graveDanger -=tmp;
            }
            cityData[i].urgency = urgency;
            cityData[i].graveDanger = graveDanger;
        }
    }
}

/* Switch units at hex pos. Put unit of index a on the top
 * of the unit stack */
void MainWind::compSwapUnit(int pos, int a){
    Unit u;
    if(a){
        u = hexagon[pos].unit[a];
        hexagon[pos].unit[a] = hexagon[pos].unit[0];
        hexagon[pos].unit[0] = u;
    }
}

/* Average attack value of an unit */
int MainWind::avgAttackVal(u_int8_t type, bool isCity){
    int sum = 0, max;
    max = isCity ? 4 : 9;
    for(int i = 0; i < max; i++)
        sum += attack.val[type][i];
    return sum/max;
}

/* How much do we want to kill an unit. Usually the function is used as follows:
 *  - benefit: how much turns does it take to produce enemy's unit.
 *  - attac: attack value of our unit against the enemy's one.
 *  - loss: how many turns does it take to produce our unit.
 *  - vuln: attack value of enemy's unit against ours.
 *  - victimCount: number of enemies on the hex */
int MainWind::killDesire(int benefit, int attac, int loss, int vuln, int victimCount){
    if(attac >= 100 && victimCount == 1)
        benefit *= 10;
    vuln *= 0.65;
    int desire = ((benefit * attac - loss * vuln) * victimCount) / (1 + attac + vuln * victimCount);
    return desire;
}

/* As bombers kill everything on a target hex and all contiguous hexes, we evaluate choice
 * by number of turns consumed to produce enemy units within the range substracted by number of
 * turns consumed to produce our units within the range */
int MainWind::bomberKillDesire(int pos, int target){
    Unit u;
    int neighbor, desire = 0;
    foreach(u, hexagon[target].unit)
        desire += (u.owner == curPlayer ? -5 : 1) * rule.prodTime[u.type];
    foreach(neighbor, hexagon[target].contig){
        if(neighbor != pos){
            foreach(u, hexagon[neighbor].unit)
                desire += (u.owner == curPlayer ? -5 : 1) * rule.prodTime[u.type];
        }
    }
    return desire;
}

/* Find nearest safe city for naval unit to repair*/
int MainWind::findNearestSafeCity(int pos, Unit *u){
    int want = 0, choice = -1, tmp;
    for(int i = 0; i < city.size(); i++){
        if(city[i].owner == curPlayer){
            tmp = findPath(pos, city[i].pos, u->type + 9).size();
            if(!tmp) continue;
            tmp /= u->type >= TRANSPORT ? maxNavalMoves(u) : rule.maxMove[u->type];
            if(tmp > 10) continue;
            tmp -= sqrt(cityData[i].graveDanger);
            if(tmp > want){
                choice = city[i].pos;
                want = tmp;
            }
        }
    }
    return choice;
}

/* Find closest point at which airborne unit (paras, fighter, bomber)
 * can replenish its fuel. We look for cities and carriers here. */
int MainWind::findNearestFuelReplenish(int pos, int carrierSize){
    int dist = 32767, choice = -1, tmp;
    for(int i = 0; i < city.size(); i++){
        if(city[i].owner == curPlayer){
            tmp = APPROXDIST(pos, city[i].pos);
            if(tmp < dist){
                choice = city[i].pos;
                dist = tmp;
            }
        }
    }
    if(hexagon[pos].unit[0].type != FIGHTER)
        return choice;
    for(int i = 0; i < carrierSize; i++){
        if(hasFriendlyShip(*carrierList[i], curPlayer, 7)){
            tmp = APPROXDIST(pos, *carrierList[i]);
            if(tmp < dist){
                choice = *carrierList[i];
                dist = tmp;
            }
        }
    }
    return choice;
}


/* Find the best city for paras unit landing operation */
int MainWind::parasMove(int pos){
    int want = 0, best = 0, target = -1, dist,
        neighbor, bestNeighbor = -1, bestDist = 32767;
    for(int i = 0; i < city.size(); i++){
        if(city[i].owner != curPlayer){
            foreach(neighbor, hexagon[city[i].pos].contig){
                dist = APPROXDIST(pos, neighbor);
                if(dist < bestDist){
                    bestDist = dist;
                    bestNeighbor = neighbor;
                }
            }
            dist = APPROXDIST(pos, bestNeighbor);
            if(!dist) continue;
            dist /= rule.maxMove[PARAS];
            want = 3 -hexagon[city[i].pos].unit.size() - dist + cityData[i].attackers;
            if(want > best){
                best = want;
                target = city[i].pos;
            }
        }
    }
    return target;
}

/* Calculate maximum moves of a naval unit. */
int MainWind::maxNavalMoves(Unit *u){
    int moves = rule.maxMove[u->type] * ceil((float)(u->HP / 100.0));
    if(!moves) moves = 1;
    return moves;
}

/* Find a first unit on a hex that hasn't been moved yet.
 * Return false if there's no such unit */
bool MainWind::findFirstUndone(int hex){
    Unit *u; int index = -1;
    for(int i = 0; i < hexagon[hex].unit.size(); i++){
        u = &hexagon[hex].unit[i];
        if(u->moves){
            index = i;
            break;
        }
    }
    if(index != -1){
        compSwapUnit(hex, index);
        return true;
    }
    return false;
}

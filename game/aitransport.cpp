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
 *  /game/aitransport.cpp
 *
 *  handle AI owned Transport units.
 *
 * *************************************************************************/

#include "../mainwind.h"

/* Manage a single transport unit. We're doing it within do-while loop,
 * so that a transport can perform more than one action in one turn */
void MainWind::compManageTransport(int i, int allySize){
    int target = -1, newPos, markAsEnded = false;
    do{
        target = transportMove(allyUnit[i], allySize);
        if(target != -1){
            newPos = moveUnit(allyUnit[i], target);
            if(allyUnit[i] == newPos)   // we cannot move anymore towards the target
                markAsEnded = true;
            allyUnit[i] = newPos;
            /* We decided to unload a tank on an island, check which tank
             * is to unload. */
            if(hexagon[newPos].unit[0].mode == ORDER_TRANSPORT_UNLOAD){
                for(int k = 1; k < hexagon[newPos].unit.size(); k++){
                    if(hexagon[target].island == hexagon[hexagon[newPos].unit[k].gotoHex].island){
                        compSwapUnit(newPos, k);
                        moveUnit(newPos, target);
                        putShipOnTop(newPos, TRANSPORT);
                        hexagon[newPos].unit[0].mode = ORDER_NOTHING;
                        break;
                    }
                }
            }
            /* Load a tank. If the tank hasn't reached the rendez-vous hex yet,
             * let the transport sit and wait at the shore */
            else if(hexagon[newPos].unit[0].mode == ORDER_TRANSPORT_PACK){
                for(int k = 0; k < hexagon[target].unit.size(); k++){
                    if(!hexagon[target].unit[k].transpReq.empty()
                            && hexagon[target].unit[k].type == TANK){
                        hexagon[target].unit[k].mode = ORDER_MOVE;
                        hexagon[target].unit[k].gotoHex =
                                hexagon[target].unit[k].transpReq.to;
                        hexagon[target].unit[k].transpReq.clear();
                        compSwapUnit(target, k);
                        moveUnit(target, newPos);
                        putShipOnTop(newPos, TRANSPORT);
                    }
                }
            }
        }
    }
    while(!markAsEnded && target != -1 && hexagon[newPos].unit[0].fuel != 0 &&
          newPos != target);
}

/* Execute a single transport movement action */
int MainWind::transportMove(int pos, int allySize){
    int min, dist, target = -1, cargoSize, neighbor, neighbor2;
    bool trRequested = false;
    Unit u;
    min = 32767;
    cargoSize = hexagon[pos].unit.size();
    /* Handle the units that are already on board. Check the closest
     * destination */
    for(int i = 1; i < cargoSize; i++){
        dist = APPROXDIST(pos, hexagon[pos].unit[i].gotoHex)
                / maxNavalMoves(&hexagon[pos].unit[0]);
        if(dist < min){
            min = dist;
            target = hexagon[pos].unit[i].gotoHex;
        }
    }
    /* Check if we have any request for hook up that is closer than that */
    if(hexagon[pos].unit.size() < MAX_NUNITS)
        for(int i = 0; i < allySize; i++){
            if(hexagon[allyUnit[i]].unit.empty()) continue;
            if(hexagon[allyUnit[i]].height <= WATER_LEVEL) continue;
            foreach(u, hexagon[allyUnit[i]].unit){
                if(u.type == TANK && !u.transpReq.empty()){
                    dist = APPROXDIST(pos, u.transpReq.where)
                            / maxNavalMoves(&hexagon[pos].unit[0]);
                    if(dist < min){
                        min = dist;
                        target = u.transpReq.where;
                        trRequested = true;
                    }
                }
            }
        }
    /* If we're not heading for any of our own units
     * requesting a hook up, find a closest shore we
     * can perform landing on */
    if(!trRequested && target != NO_TARGET){
        hexagon[pos].unit[0].mode = ORDER_TRANSPORT_UNLOAD;
        if(canAllocateNewUnit(target, TANK, curPlayer)
                || (hexagon[target].terrain == 1
                && !hasFriendlyCity(target, curPlayer)
                    && hexagon[target].unit.size()
                    <= hexagon[pos].unit.size() - 1))
            return target;
        else{
            /* Try unloading on contiguous hex*/
            foreach(neighbor, hexagon[target].contig)
                if(isShore(neighbor) &&
                        canAllocateNewUnit(neighbor, TANK, curPlayer))
                         return neighbor;
            /* If that's not possible, widen the range to 2 hexes */
            foreach(neighbor, hexagon[target].contig)
                foreach(neighbor2, hexagon[neighbor].contig)
                    if(neighbor2 != target && isShore(neighbor2)
                            && canAllocateNewUnit(neighbor2, TANK, curPlayer))
                        return neighbor2;
            /* If that is still not possible, use brute-force BFS */
            return unloadUnitNearby(target, curPlayer);
        }
    }
    else if(trRequested && target != NO_TARGET){
        hexagon[pos].unit[0].mode = ORDER_TRANSPORT_PACK;
        return target;
    }
    return NO_TARGET;
}

/* Procedure for finding closest possible hex for unloading
 * tanks carried by transports. This is based on BFS algorithm */
int MainWind::unloadUnitNearby(int pos, u_int8_t owner){
    int u, neighbor, target = NO_TARGET, best = 32767, dist;
    QQueue<int> Q;
    QSet<int> S;
    Q.append(pos);
    S.insert(pos);
    while(!Q.empty()){
        u = Q.front();
        Q.pop_front();
        /* We can unload here: check if it's better than the previous choice */
        if(canAllocateNewUnit(u, TANK, owner)){
            dist = APPROXDIST(pos, u);
            if(dist > 4) continue;
            if(dist < best){
                best = dist;
                target = u;
            }
        }
        foreach(neighbor, hexagon[u].contig)
            if(!S.contains(neighbor) && hexagon[neighbor].height < NOACCESS_LEVEL
                    && isShore(neighbor)){
                Q.append(neighbor);
                S.insert(neighbor);
            }
    }
    S.clear();
    return target;
}

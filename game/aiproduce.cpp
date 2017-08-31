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
 *  Handle productions in AI controlled cities.
 *
 * *************************************************************************/

#include "../mainwind.h"

void MainWind::compProdAllCities(int *cityCount,int allySize, int enemySize, int transSize){
    int i;
    /* Count interesting objects on every island */
    countObjects(allySize, enemySize);
    for(i = 0; i < city.size(); i++)
        if(city[i].owner == curPlayer)
            /* If AI had produced an unit in previous turn, it may change production */
            if(city[i].produces == NOPRODUCTION)
                compProduce(i, cityCount, transSize);
}

/* Set production in a single city */
void MainWind::compProduce(int where, int *cityCount, int transSize){
    bool wantTransports = true;
    int ratio, bestProd, interest, needCount, i;
    int island = hexagon[city[where].pos].island;
    /* See if we have anything of interest on the island */
    interest = (scanCounts[island].unexplored || scanCounts[island].enemyCities
            || scanCounts[island].enemyUnits[0] || scanCounts[island].neutCities);
    /* How much do AI need to set production to tanks */
    needCount = scanCounts[island].enemyCities - scanCounts[island].allyTankProducers + interest;
    if(scanCounts[island].enemyCities) needCount++;

    /*If there's a need for an army producer */
    if(needCount > 0){
        setCompProduction(where, TANK, cityCount);
        scanCounts[island].allyTankProducers++;
        return;
    }

    /* If production of other unit just have started and there are enemy cities to conquer on the island */
    if(scanCounts[island].enemyCities && city[where].produces == NOPRODUCTION){
        setCompProduction(where, TANK, cityCount);
        scanCounts[island].allyTankProducers++;
        return;
    }

    /* We have 4 kinds of ratios, see ProdRules in /game/pieceRules.cpp for exact values */
    ratio = (player[curPlayer].citiesControl - 1) / 5;
    if(ratio > 3) ratio = 3;

    if(player[curPlayer].citiesControl == 1 && city[where].produces == TANK)
        return;

    if(transSize > player[curPlayer].citiesControl){
        wantTransports = false;
        goto setCustom;
    }
    /* If there's no transport producers, switch production to transports */
    if(cityCount[TRANSPORT] == 0){
        setCompProduction(where, TRANSPORT, cityCount);
        return;
    }

    /* If there's something on the continent AI is interested in, and we already produce tanks,
     * don't change this production */
    if(city[where].produces == TANK && interest) return;

    /* Produce tank if there's neutral or enemy city */
    if(scanCounts[island].neutCities && city[where].produces == NOPRODUCTION){
        setCompProduction(where, TANK, cityCount);
        scanCounts[island].allyTankProducers++;
        return;
    }

    /* Set production to most needed unit, either if:
     *  - there's no undergoing production,
     *  - there's overproduction of certain type of unit,
     *  - there's only 1 city on island and it produces tanks */
    setCustom:
    interest = (scanCounts[island].allyCities != 1 || interest);
    if(city[where].produces == NOPRODUCTION
            || overproduction(where, ratio, cityCount)
            || (city[where].produces == 1 && scanCounts[island].allyCities == 1)){
        //if(!interest) cityCount[0] = 32767;
        /* Set the best production option... */
        bestProd = 0;
        for(i = !interest ? 1 : 0; i < 9; i++){
            if(!wantTransports && i == 4) continue;
            bestProd = (cityCount[bestProd] * prodRatio.val[ratio][i] <=
                    cityCount[i] * prodRatio.val[ratio][bestProd]) ? bestProd : i;
        }
        /* ...and apply it */
        setCompProduction(where, bestProd, cityCount);
        if(!bestProd) scanCounts[island].allyTankProducers++;
    }

}

/* Set production to particular city */
void MainWind::setCompProduction(int where, u_int8_t type, int *cityCount){
    cityCount[type]++;
    city[where].produces = type;
    city[where].timeToProduce = rule.prodTime[type] * prodFactor(curPlayer);
    if(!city[where].timeToProduce)
        city[where].timeToProduce++;
}

/* Check if there's an overproduction of any of the kind of unit */
bool MainWind::overproduction(int where, int ratio, int *cityCount){
    for(int i = 0; i < 9; i++){
        if(city[where].produces != i &&
                (cityCount[city[where].produces] - 1) * prodRatio.val[ratio][i]
                > (cityCount[i] + 1) * prodRatio.val[ratio][city[where].produces])
            return true;
    }
    return false;
}

/* Easy AI produces pieces 50% slower, hard AI
 * 50% faster. Medium AI and humans have normal speed */
float MainWind::prodFactor(u_int8_t i){
    if(player[i].level == 2) return 1.5;
    else if(player[i].level == 4) return 0.5;
    else return 1.0;
    return 0;
}

/* Function to count various objects on a logical island:
 * allied, neutral and enemy cities, allied and enemy units,
 * undiscovered hexes */
void MainWind::countObjects(int allySize, int enemySize){
    Unit u;
    int island;
    for(int i = 0; i < NOF_ISLANDS; i++)
        scanCounts[i].clear();
    for(int i = 0; i < allySize; i++){
        if(hexagon[allyUnit[i]].island == -1) continue;
        island = hexagon[allyUnit[i]].island;
        foreach(u, hexagon[allyUnit[i]].unit)
            scanCounts[island].allyUnits[u.type]++;
    }
    for(int i = 0; i < enemySize; i++){
        if(hexagon[enemyUnit[i]].island == -1) continue;
        island = hexagon[enemyUnit[i]].island;
        foreach(u, hexagon[enemyUnit[i]].unit)
            scanCounts[island].enemyUnits[u.type]++;
    }
    for(int i = 0; i < city.size(); i++){
        island = hexagon[city[i].pos].island;
        if(city[i].owner == curPlayer){
            scanCounts[island].allyCities++;
            if(city[i].produces == TANK)
                scanCounts[island].allyTankProducers++;
        }
        else if(city[i].owner == 6)
            scanCounts[island].neutCities++;
        else scanCounts[island].enemyCities++;
    }
    // Don't count unexplored hexes yet. */
    /*for(int i = 0; i < SIZEOF_MAP; i++)
        if(hexagon[i].island != -1 &&
                !isHexUncovered(i, curPlayer))
            scanCounts[hexagon[i].island].unexplored++;*/

}



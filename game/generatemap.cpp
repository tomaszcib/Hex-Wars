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
 *  /game/generatemap.cpp
 *
 *  functions used to generate worlds and populate them with cities and players.
 *
 * *************************************************************************/

#include "../mainwind.h"

void MainWind::generateIsland(int size, int landAmount){
    QPoint pos;
    int a, b;
    LOG(INFO, str.log[8]);
    SIZEOF_MAP = size * size;
    rows = sqrt(SIZEOF_MAP), cols = sqrt(SIZEOF_MAP);
    pixelWidth = sizeInHexes(cols);
    for(int i = 0; i < SIZEOF_MAP; i++){
        hexagon[i].height = 0;
        hexagon[i].visible = 128;
        hexagon[i].discovered = 128;
        hexagon[i].fScore = 0;
        hexagon[i].gScore = 0;
        hexagon[i].island = -1;
        hexagon[i].region = -1;
        hexagon[i].terrain = 0;
        hexagon[i].cityId = 0;
        hexagon[i].vis = 0;
        pos = hexPosition(i);
        createHexItem(hexagonPath, pos);
    }

    /* Find contingencies between hexes */
    for(int i = 0; i < SIZEOF_MAP; i++){
        /* Right edge - odd row */
        if(!((i+1) % cols) && ((i / rows) % 2)){
            if(hexExists(i+cols)) hexagon[i].contig.append(i+cols);
            if(hexExists(i-cols)) hexagon[i].contig.append(i-cols);
            hexagon[i].contig.append(i-1);
        }
        /* Left edge - even row */
        else if(!(i % cols) && !((i / rows) % 2)){
            if(hexExists(i+cols)) hexagon[i].contig.append(i+cols);
            if(hexExists(i-cols)) hexagon[i].contig.append(i-cols);
            hexagon[i].contig.append(i+1);
        }
        else{
            /* If it's not left edge, odd row */
            if(!(!(i % cols) && ((i / rows) % 2))) hexagon[i].contig.append(i-1);
            /* If it's not right edge, even row */
            if(!(!((i+1) % cols) && !((i / rows) % 2))) hexagon[i].contig.append(i+1);
            if(hexExists(i-cols)) hexagon[i].contig.append(i-cols);
            if(hexExists(i+cols)) hexagon[i].contig.append(i+cols);
            /* If it's in even row */
            if((i / rows) % 2){
                if(hexExists(i-cols+1)) hexagon[i].contig.append(i-cols+1);
                if(hexExists(i+cols+1)) hexagon[i].contig.append(i+cols+1);
            }
            /* If it's in odd row */
            else{
                if(hexExists(i-cols-1)) hexagon[i].contig.append(i-cols-1);
                if(hexExists(i+cols-1)) hexagon[i].contig.append(i+cols-1);

            }
        }
    }

    /* Determine island heightmap */
    for(int i = 0; i < landAmount; i++){
        a = R.rand()%rows;
        b = R.rand()%cols;
        gaussianHeight(2, a * rows + b, sizeInHexes((R.rand()%2)+1), sizeInHexes((R.rand()%2)+1));
    }

    createMinimap();
    logicalIslandGen();
    spawnCities();
    logicalRegionGen();
}

/* A logical island is a set of land hexes in which
 * for every a, b belonging to island I, there is a land path
 * which belongs to island I */
void MainWind::logicalIslandGen(){
    QQueue<int> Q;
    int u;
    LOG(INFO, str.log[9]);
    NOF_ISLANDS = 0;
    for(int i = 0; i < SIZEOF_MAP; i++){
        hexagon[i].island = -1;
        hexagon[i].region = -1;
    }
    for(int i = 0; i < SIZEOF_MAP; i++){
        if(hexagon[i].height > WATER_LEVEL && hexagon[i].height <= NOACCESS_LEVEL && hexagon[i].island == -1){
            Q.clear();
            Q.append(i);
            NOF_ISLANDS++;
            while(!Q.empty()){
                u = Q.front();
                Q.pop_front();
                hexagon[u].island = NOF_ISLANDS - 1;
                for(int v = 0; v < hexagon[u].contig.size(); v++){
                    if(hexagon[hexagon[u].contig[v]].height > WATER_LEVEL &&
                            hexagon[hexagon[u].contig[v]].height <= NOACCESS_LEVEL &&
                            hexagon[hexagon[u].contig[v]].island == -1)
                        Q.append(hexagon[u].contig[v]);
                }
            }
        }
    }
    /* Determine size in hexes of every island */
    islandSize.resize(NOF_ISLANDS);
    scanCounts.resize(NOF_ISLANDS);
    for(int i = 0; i < NOF_ISLANDS; i++)
        islandSize[i] = 0;
    for(int i = 0; i < SIZEOF_MAP; i++)
        if(hexagon[i].island != -1)
            islandSize[hexagon[i].island]++;

}

/* Do BFS one more time - this time to spawn cities.
 * Cities are spawned on shores only, around one per every 8 hexes
 * of the island */
void MainWind::spawnCities(){
    int u, v, howManyCities;
    bool isShore;
    QQueue<short int> Q;
    QVector<short int> islandShore;
    City tmp;
    LOG(INFO, str.log[11]);
    for(int i = 0; i < SIZEOF_MAP; i++)
        hexagon[i].vis = 0;

    for(int i = 0; i < SIZEOF_MAP; i++){
        if(hexagon[i].island != -1 && !hexagon[i].vis){
            Q.clear();
            Q.append(i);
            while(!Q.empty()){
                u = Q.front();
                Q.pop_front();
                isShore = false;
                for(int v = 0; v < hexagon[u].contig.size(); v++){
                    if(hexagon[hexagon[u].contig[v]].island == hexagon[u].island && !hexagon[hexagon[u].contig[v]].vis){
                        Q.append(hexagon[u].contig[v]);
                        hexagon[hexagon[u].contig[v]].vis = 1;
                    }
                    if(hexagon[hexagon[u].contig[v]].height <= WATER_LEVEL)
                        isShore = true;
                }
                if(isShore) islandShore.append(u);
            }
            howManyCities = islandSize[hexagon[i].island]/8;
            for(int k = 0; k < howManyCities; k++){
                u = (R.rand()%islandShore.size());
                hexagon[islandShore[u]].terrain = 1;
                islandShore.remove(u);
            }
            islandShore.clear();
        }
    }

    /* Make sure there aren't any contiguous cities
     * and calculate sum of cities for every island */
    islandCityCount.resize(NOF_ISLANDS);
    for(int i = 0; i < NOF_ISLANDS; i++) islandCityCount[i] = 0;

    for(int i = 0; i < SIZEOF_MAP; i++){
        if(hexagon[i].terrain == 1){
            foreach(u, hexagon[i].contig)
                foreach(v, hexagon[u].contig)
                    if(hexagon[v].terrain == 1 && v != i)
                        hexagon[v].terrain = 0;

            /* Add the city to the global vector of cities */
            islandCityCount[hexagon[i].island]++;
            tmp.pos = i;
            tmp.owner = 6;
            tmp.produces = 0;
            tmp.timeToProduce = 0;
            city.append(tmp);
            hexagon[i].cityId = city.size() - 1;
        }
    }
    cityData.resize(city.size());
}

/* A logical region aligned to the city C is a set of hexes
 * whose distance to the city C is smaller than distance to any
 * other city */
void MainWind::logicalRegionGen(){
    LOG(INFO, str.log[10]);
    double minimum = INFINITY, tmpDist;
    for(int i = 0; i < SIZEOF_MAP; i++){
        minimum = INFINITY;
        for(int j = 0; j < city.size(); j++){
            tmpDist = distanceBetweenHexes(i, city[j].pos);
            if(tmpDist < minimum) {
                minimum = tmpDist;
                hexagon[i].region = j;
            }
        }
    }
}

/* p - point around which we want to create gaussian hill */
void MainWind::gaussianHeight(double A, int base, double sigmaX, double sigmaY){
    QPoint pos, p;
    double val;
    double a = 1/(2*sigmaX*sigmaX);
    double c = 1/(2*sigmaY*sigmaY);
    p = hexPosition(base);
    for(int i = 0; i < SIZEOF_MAP; i++){
        pos = hexPosition(i);
        val = A * exp( -a*(pos.x()-p.x())*(pos.x()-p.x()) - c*(pos.y()-p.y())*(pos.y()-p.y()) );
        /* Modelling height */
        hexagon[i].height += val;
    }
}

/* Every player gets one city and one tank at the beginning of the game,
 * pick starting locations randomly */
void MainWind::spawnStartingPos(){
    int startCity;
    LOG(INFO, str.log[12]);
    lastHumanPlayer = 6;
    for(int i = 0; i < 6; i++){
        do{
            startCity = R.rand() % city.size();
        } while(hexagon[city[startCity].pos].vis == 255);
        hexagon[city[startCity].pos].vis = 255;
        if(player[i].isActive){
            captureCity(startCity, i);
            city[startCity].produces = player[i].isHuman ? TANK : NOPRODUCTION;
            city[startCity].timeToProduce = player[i].isHuman ? rule.prodTime[0] : 255;
            setHexUncovered(city[startCity].pos, i);
            spawnUnit(city[startCity].pos, newUnit(TANK, i));
            hexagon[city[startCity].pos].vis = 255;
        }
    }
}

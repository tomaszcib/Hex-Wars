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
 *  core.cpp
 *
 *  essential functions for hex geometry, visibility and conditions;
 *  QVector priority queue implemented into pathfinding algorithm (see:
 *  /game/move.cpp for more details); other miscellaneous procedures.
 *
 * *************************************************************************/

#include "mainwind.h"

/* Operations on hexagons */
void MainWind::createHexagonPath(){
    hexPolygon.append(QPoint(0, 0));
    hexPolygon.append(QPoint((HEX_SIZE * sqrt(3)) / 2 + 1, 0.5 * HEX_SIZE));
    hexPolygon.append(QPoint((HEX_SIZE * sqrt(3)) / 2 + 1, 1.5 * HEX_SIZE));
    hexPolygon.append(QPoint(0, 2 * HEX_SIZE));
    hexPolygon.append(QPoint(-(HEX_SIZE * sqrt(3)) / 2 - 1, 1.5 * HEX_SIZE));
    hexPolygon.append(QPoint(-(HEX_SIZE * sqrt(3)) / 2 - 1, 0.5 * HEX_SIZE));
    hexPolygon.append(QPoint(0, 0));
    hexagonPath.addPolygon(hexPolygon);
}

QPoint MainWind::hexRelativeCoords(int base, int target){
    QPoint basePos, targetPos;
    basePos = hexPosition(base);
    targetPos = hexPosition(target);
    return QPoint(targetPos.x() - basePos.x(), targetPos.y() - basePos.y());
}

double MainWind::sizeInHexes(double howMany){
    return howMany * HEX_SIZE * sqrt(3);
}

bool MainWind::hexExists(int i){
    if(i < 0 || i >= SIZEOF_MAP) return false;
    else return true;
}

QPoint MainWind::hexPosition(int base){
    int i, j;
    i = base / rows;
    j = base % rows;
    return QPoint( ceil((i%2)*(HEX_SIZE*sqrt(3)/2) + HEX_SIZE*sqrt(3)*j), i * 1.5 * HEX_SIZE);
}

int MainWind::hexWhichDirection(int base, int p){
    QPoint a, b;
    a = hexPosition(base);
    b = hexPosition(p);
    if(b.x() < a.x() && b.y() < a.y()) return 0;
    else if(b.x() > a.x() && b.y() < a.y()) return 1;
    else if(b.x() > a.x() && b.y() == a.y()) return 5;
    else if(b.x() > a.x() && b.y() > a.y()) return 3;
    else if(b.x() < a.x() && b.y() > a.y()) return 2;
    else if(b.x() < a.x() && b.y() == a.y()) return 4;
    return -1;
}

double MainWind::distanceBetweenHexes(int a, int b){
    QPoint p = hexRelativeCoords(a, b);
    return (p.x()*p.x() + p.y()*p.y());
}

bool MainWind::isContigToAlly(int i, u_int8_t player){
    int neighbor;
    foreach(neighbor, hexagon[i].contig){
        if(hexagon[neighbor].terrain == 1)
            if(city[hexagon[neighbor].cityId].owner == player)
                return true;
        if(!hexagon[neighbor].unit.empty())
            if(hexagon[neighbor].unit[0].owner == player)
                return true;
    }
    return false;
}

bool MainWind::isShore(int i){
    int neighbor;
    if(hexagon[i].height <= WATER_LEVEL) return false;
    foreach(neighbor, hexagon[i].contig)
        if(hexagon[neighbor].height <= WATER_LEVEL)
            return true;
    return false;
}

bool MainWind::isContainingAlly(int i, u_int8_t player){
    if(!hexagon[i].unit.empty())
        if(hexagon[i].unit[0].owner == player)
            return true;
    return hasFriendlyCity(i, player);
}

bool MainWind::isInFriendlyControl(int i, u_int8_t player){
    int neighbor;
    foreach(neighbor, hexagon[i].contig)
        if(isContigToAlly(neighbor, player))
            return true;
    return false;
}

bool MainWind::hasFriendlyCity(int i, u_int8_t player){
    if(hexagon[i].terrain == 1)
        if(city[hexagon[i].cityId].owner == player)
            return true;
    return false;
}

bool MainWind::isContigToFriendlyCity(int i, u_int8_t player){
    int neighbor;
    foreach(neighbor, hexagon[i].contig)
        if(hasFriendlyCity(neighbor, player))
            return true;
    return false;
}

bool MainWind::hasFriendlyShip(int i, u_int8_t player, u_int8_t type){
    Unit ship;
    foreach(ship, hexagon[i].unit)
        if(ship.owner == player && ship.type == type)
            return true;
    return false;
}

bool MainWind::hasEnemyUnit(int i, u_int8_t player){
    if(!hexagon[i].unit.empty())
        if(hexagon[i].unit[0].owner != player)
            return true;
    return false;
}

void MainWind::putShipOnTop(int i, u_int8_t type){
    Unit tmpSwap;
    for(int j = 1; j < hexagon[i].unit.size(); j++)
        if(hexagon[i].unit[j].type == type){
            tmpSwap = hexagon[i].unit[j];
            hexagon[i].unit[j] = hexagon[i].unit[0];
            hexagon[i].unit[0] = tmpSwap;
            return;
        }
}

void MainWind::unitDisappear(int hex, int depth, u_int8_t player){
    int neighbor;
    if(!isInFriendlyControl(hex, player)){
        setHexInvisible(hex, player);
        if(player == lastHumanPlayer)
            paintHex(hex);
    }
    if(depth < 2)
        foreach(neighbor, hexagon[hex].contig)
            unitDisappear(neighbor, depth + 1, player);
}

void MainWind::unitAppear(int hex, int depth, u_int8_t player){
    int neighbor;
    setHexVisible(hex, player);
    setHexUncovered(hex, player);
    if(player == lastHumanPlayer)
        paintHex(hex);
    if(depth < 2)
        foreach(neighbor, hexagon[hex].contig)
            unitAppear(neighbor, depth + 1, player);
}

/* Function for clearing up ScanCounts values */
void ScanCounts::clear(){
    allyCities = 0;
    allyTankProducers = 0;
    enemyCities = 0;
    neutCities = 0;
    unexplored = 0;
    for(int k = 0; k < 9; k++){
        allyUnits[k] = 0;
        enemyUnits[k] = 0;
    }
}

int MainWind::findShore(int pos){
    int neighbor, u;
    QQueue<int> Q;
    Q.append(pos);
    for(int i = 0; i < SIZEOF_MAP; i++)
        hexagon[i].vis = 0;
    while(!Q.empty()){
        u = Q.front();
        hexagon[u].vis = 1;
        Q.pop_front();
        if(isShore(u)) { Q.clear(); return u; }
        foreach(neighbor, hexagon[u].contig)
            if(hexagon[neighbor].island == hexagon[u].island
                    && !hexagon[neighbor].vis)
                Q.append(neighbor);
    }
    return -1;
}



/* Hexagon visibility functions */
void MainWind::setHexVisible(int i, u_int8_t player){
    hexagon[i].visible |= (1 << player);
}

void MainWind::setHexInvisible(int i, u_int8_t player){
    hexagon[i].visible &= ~(1 << player);
}

void MainWind::setHexUncovered(int i, u_int8_t player){
    hexagon[i].discovered |= (1 << player);
}

bool MainWind::isHexUncovered(int i, u_int8_t player){
    return (hexagon[i].discovered & (1 << player));
}

bool MainWind::isHexVisible(int i, u_int8_t player){
    return (hexagon[i].visible & (1 << player));
}


/* Functions for integer priority queue on QVector - calculating  */
void MainWind::PQinsert(QVector<int> &Q, int i){
    int j, tmp;
    Q.append(i);
    j = Q.size() - 1;
    while(j > 0 && hexagon[Q[j/2]].fScore > hexagon[Q[j]].fScore){
        tmp = Q[j/2];
        Q[j/2] = Q[j];
        Q[j] = tmp;
        j /= 2;
    }
}

int MainWind::PQfront(QVector<int> &Q){
    int returnVal;
    if(Q.empty()) return -1;
    returnVal = Q[0];
    Q.pop_front();
    PQheapify(Q, 0);
    return returnVal;
}

void MainWind::PQheapify(QVector<int> &Q, int i){
    int l, r, pivot;
    l = 2 * i + 1;
    r = 2 * i + 2;
    if(l < Q.size() && hexagon[Q[l]].fScore < hexagon[Q[i]].fScore)
        pivot = l;
    else pivot = i;
    if(r < Q.size() && hexagon[Q[r]].fScore < hexagon[Q[pivot]].fScore)
        pivot = r;
    if(pivot != i){
        l = Q[pivot];
        Q[pivot] = Q[i];
        Q[i] = l;
        PQheapify(Q, pivot);
    }
}

//TIME DEBUG DELAY
void MainWind::delay(){
    QTime dieTime = QTime::currentTime().addMSecs(25);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/* Fisher-Yates shuffle of int array */
void MainWind::shuffleList(u_int16_t *list, int size){
    int tmp, j;
    for(int i = size - 1; i >= 1; i--){
        j = R.rand() % (i + 1);
        tmp = list[i];
        list[i] = list[j];
        list[j] = tmp;
    }
}

void MainWind::countPlayerUnits(){
    Unit u;
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 9; j++)
            player[i].unitsContType[j] = 0;
    for(int i = 0; i < SIZEOF_MAP; i++){
        foreach(u, hexagon[i].unit)
            player[u.owner].unitsContType[u.type]++;
    }
}

/* Functions for TransportRequest class */
void TransportRequest::clear(){
    where = to = waitTime = urgency = -1;
}

bool TransportRequest::empty(){
    if(where == -1 && to == -1 && waitTime == -1 && urgency == -1)
        return true;
    else return false;
}

void TransportRequest::setTarget(int Where, int To, int WaitTime, int Urgency){
    where = Where;
    to = To;
    waitTime = WaitTime;
    urgency = Urgency;
}

/* Create ranking for players of chosen type */
void MainWind::createRanking(u_int8_t type, u_int8_t *table){
    u_int8_t i, j, rank;
    /* Simple count-sort. type==1 is cities ranking, type==0 is units ranking */
    for(i = 0; i < 6; i++){
        rank = 0;
        for(j = 0; j < 6; j++)
            if((type ? player[i].citiesControl : player[i].unitsControl)
                    < (type ? player[j].citiesControl : player[j].unitsControl))
                rank++;
        table[i] = rank;
    }
}

/* Print string onto stdout stream and "Program log" window */
void MainWind::LOG(u_int8_t type, QString strng){
    QTextStream out(stdout);
    strng = "[" + QTime::currentTime().toString("hh:mm:ss") + " " + str.string[114 + type] + "] " + strng + "\n";
    logWindow.append(strng);
    out << strng;
    out.flush();
}

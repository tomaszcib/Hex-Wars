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
 *  struct.h
 *
 *  structures for various assets of the game. I didn't add commentary to them,
 *  but it should be clear what they are supposed to mean.
 *
 * *************************************************************************/

#ifndef PROVINCE_H
#define PROVINCE_H

#include <QtWidgets>
//#include <QtGui>
#include "const.h"

struct City{
    u_int8_t produces, timeToProduce, owner;
    int pos;
};

class TransportRequest{
public:
    void clear();
    bool empty();
    void setTarget(int Where, int To, int WaitTime, int Urgency);
    int where;
    int to;
    int8_t waitTime;
    int urgency;
};

struct TransportTask{
    int pos, target;
    bool mode;
};

struct BodyguardRequest{
    int where;
    int needed;
    int urgency;
};

struct Unit{
    u_int8_t type, owner, moves, HP, fuel, mode;
    int gotoHex;
    TransportRequest transpReq;
};

struct Hexagon{
    QVector<short int> contig;
    QVector<Unit> unit;
    u_int8_t terrain;
    short int cityId;
    double height;
    u_int8_t vis, discovered, visible;
    short int island, region;
    double gScore, fScore;
    QImage texture;
};

struct Minimap{
    QImage image;
    int squareSize;
    float pixelScale;
};

struct Player{
    int level;
    int citiesControl, unitsControl, unitsContType[9];
    bool isHuman, isActive;
    unsigned int scoreKilled, scoreProduced;
    QRgb color;
    QList<BodyguardRequest> bgRequest;
};

class ScanCounts{
public:
    void clear();
    int allyCities;
    int allyTankProducers;
    int allyUnits[9];
    int enemyCities;
    int enemyUnits[9];
    int neutCities;
    int unexplored;
};

struct AiCity{
    int id;
    int attack, danger;
    int graveDanger, urgency;
    int attackers, occupy;
};

struct HighScoreItem{
    QString name;
    u_int32_t world, turns, score;
    u_int8_t size, land, diff, color;
};

#endif // PROVINCE_H

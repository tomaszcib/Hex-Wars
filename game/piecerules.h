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
 *  /game/piecerules.h
 *
 *  definitions of PieceRules and AttackRules classes. See /game/pieceRules.cpp
 *  file for details.
 *
 * *************************************************************************/

#ifndef PIECERULES_H
#define PIECERULES_H
#include <QtWidgets>
//#include <QtGui>

class PieceRules{
public:
    PieceRules();
    void setDefaultRules();
    u_int8_t prodTime[9], maxMove[9], maxFuel[9];
};

class AttackRules{
public:
    AttackRules();
    u_int8_t val[9][9];
};

class ProdRules{
public:
    ProdRules();
    u_int8_t val[4][9];
};

#endif

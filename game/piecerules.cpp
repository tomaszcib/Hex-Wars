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
 *  /game/piecerules.cpp
 *
 *  file associated with /game/piecerules.h header file.
 *
 * *************************************************************************/

#include "piecerules.h"

/* Sets default ruleset */

PieceRules::PieceRules(){
    setDefaultRules();
}

void PieceRules::setDefaultRules(){
    prodTime[0] = 4;
    prodTime[1] = 6;
    prodTime[2] = 8;
    prodTime[3] = 12;
    prodTime[4] = 2;
    prodTime[5] = 6;
    prodTime[6] = 12;
    prodTime[7] = 10;
    prodTime[8] = 6;

    maxMove[0] = 1;
    maxMove[1] = 8;
    maxMove[2] = 8;
    maxMove[3] = 8;
    maxMove[4] = 4;
    maxMove[5] = 6;
    maxMove[6] = 4;
    maxMove[7] = 3;
    maxMove[8] = 4;

    maxFuel[1] = 4;
    maxFuel[2] = 2;
    maxFuel[3] = 4;
}

/* Set attack values for all of the units */
AttackRules::AttackRules(){
    u_int8_t tmp[9][9] = { {33, 20, 20, 33, 25, 5, 5, 5, 5},
                           {0, 0, 0, 0, 0, 0, 0, 0, 0},
                           {20, 255, 33, 255, 50, 10, 10, 10, 33},
                           {255, 255, 255, 255, 255, 255, 255, 255, 255},
                           {0, 0, 0, 0, 0, 0, 0, 0, 0},
                           {5, 20, 50, 33, 255, 30, 5, 10, 100},
                           {100, 100, 100, 100, 100, 100, 20, 33, 33},
                           {0, 33, 50, 50, 20, 20, 5, 5, 20},
                           {0, 0, 5, 10, 255, 20, 100, 100, 20} };
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++)
            val[i][j] = tmp[i][j];
}

/* Set ratios for AI production - used in /game/compmove.cpp */
ProdRules::ProdRules(){
    u_int8_t tmp[4][9] = { {30, 0, 10, 5, 15, 10, 10, 0, 10},
                           {70, 10, 10, 5, 25, 20, 10, 10, 10},
                           {120, 20, 30, 10, 50, 10, 10, 10, 10},
                           {150, 30, 30, 10, 70, 20, 10, 10, 20} };
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 9; j++)
            val[i][j] = tmp[i][j];
}

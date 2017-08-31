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
 *  const.h
 *
 *  preprocessor macros definitions, global constants for the program.
 *
 * *************************************************************************/

#ifndef CONST_H
#define CONST_H

#define HEX_SIZE 24
#define TEXTURE_WIDTH 43
#define TEXTURE_HEIGHT 48
#define NOF_TEXTURES 22
#define UNIT_SIZE 30

#define WATER_LEVEL 1.2
#define NOACCESS_LEVEL 2.25

#define TEXTURE_GRASS 0
#define TEXTURE_WATER 1
#define TEXTURE_FOW 2
#define TEXTURE_MOUNT 9

#define PLAYER_RED 0xffdd0000
#define PLAYER_GREEN 0xff00dd00
#define PLAYER_BLUE 0xff0000dd
#define PLAYER_YELLOW 0xffdddd00
#define PLAYER_CYAN 0xff00dddd
#define PLAYER_MAGENTA 0xffdd00dd

#define MIN_MAPSIZE 40
#define MAX_MAPSIZE 70

#define MAX_UNITS 6
#define MAX_NUNITS 7

#define TANK 0
#define PARAS 1
#define FIGHTER 2
#define BOMBER 3
#define TRANSPORT 4
#define DESTROYER 5
#define BATTLESHIP 6
#define CARRIER 7
#define SUBMARINE 8

#define NOPRODUCTION 255

#define ORDER_NOTHING 0
#define ORDER_LOAD 1
#define ORDER_MOVE 2
#define ORDER_WAIT_TRANSPORT 3
#define ORDER_TRANSPORT_UNLOAD 4
#define ORDER_TRANSPORT_PACK 5
#define ORDER_PARAS_MOVE 6
#define ORDER_PARAS_ATTACK 7
#define ORDER_EXPLORE 8

#define NO_TARGET -1

#define HAS_NO_UNIT 0
#define HAS_ALLY_UNIT 1
#define HAS_ALLY_TRAN 2
#define HAS_ALLY_CARR 3
#define HAS_ENEMY_UNIT 4

#define GAME 0
#define INFO 1
#define WARN 2
#define ERROR 3
#define DEBUG 4

/* Macos section */
#define APPROXDIST(A,B) ceil(sqrt(distanceBetweenHexes((A), (B))) / 42)
#define ISAIR(A) ((A) >= 1 && (A) <= 3) ? 1 : 0
#define ISNAVAL(A) ((A) > 3) ? 1 : 0

#endif // CONST_H

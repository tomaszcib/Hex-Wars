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
 *  rand.h
 *
 *  definition of Random class. See rand.cpp for more details.
 *
 * *************************************************************************/

#ifndef RAND_H
#define RAND_H

#include <QtGlobal>

class Random{
public:
    Random();
    u_int32_t rand();
    void srand(u_int32_t seed);
    u_int32_t getSrand();
private:
    u_int32_t x, y, z, w, v, srandSeed;
};

#endif // RAND_H

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
 *  rand.cpp
 *
 *  Platform-independent pseudo-random number generator (PRNG). The algorithm
 *  has originally been presented by George Marsaglia here:
 *  https://groups.google.com/forum/#!msg/comp.lang.c/qZFQgKRCQGg/rmPkaRHqxOMJ
 *  Although k parameter used by me equals 5, it seems to be sufficient as for
 *  needs of this game.
 *
 *  Please be careful while modding this file and note that
 *  any change to it would most likely lead to inability to load .SAV files
 *  from other versions of the game.
 *
 * *************************************************************************/

#include "rand.h"

Random::Random(){
    x = 123456789;
    y = 362436069;
    z = 521288629;
    w = 88675123;
    v = 886756453;
}

u_int32_t Random::rand(){
    u_int32_t t;
    t = (x^(x>>7));
    x=y, y=z, z=w, w=v;
    v = (v^(v<<6)) ^ (t^(t<<13));
    return (y+y+1)*v;
}

void Random::srand(u_int32_t seed){
    srandSeed = seed;
    x = seed;
    y = 362436069;
    z = 521288629;
    w = 88675123;
    v = 886756453;
}

u_int32_t Random::getSrand(){
    return srandSeed;
}

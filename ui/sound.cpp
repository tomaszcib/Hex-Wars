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
 *  /ui/sound.cpp
 *
 *  Sound class constructor. See /ui/sound.h header file for more information.
 *
 * *************************************************************************/

#include "sound.h"

Sound::Sound(){
    mLand = Mix_LoadMUS("./audio/foot3.ogg");
    mAir = Mix_LoadMUS("./audio/air4.ogg");
    mNaval = Mix_LoadMUS("./audio/inh2o.ogg");
    attack[0] = Mix_LoadMUS("./audio/MgBar1.ogg");
    attack[1] = NULL;
    attack[2] = Mix_LoadMUS("./audio/MgBar2.ogg");
    attack[3] = NULL;
    attack[4] = NULL;
    attack[5] = Mix_LoadMUS("./audio/MedCan.ogg");
    attack[6] = Mix_LoadMUS("./audio/LrgCan.ogg");
    attack[7] = Mix_LoadMUS("./audio/MedCan.ogg");
    attack[8] = Mix_LoadMUS("./audio/MedCan.ogg");
    destroy = Mix_LoadMUS("./audio/SmlExpl.ogg");
    capture = Mix_LoadMUS("./audio/wall01.ogg");
}

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
 *  /ui/sound.h
 *
 *  Sound class header. This class contains sounds played on various events
 *  within the game.
 *
 *  Please note that SDL and SDL_mixer libraries are needed to compile the game
 *  properly. If they are not available in your distro's repository, you can
 *  download and install them via SDL's official website:
 *  https://www.libsdl.org/index.php
 *
 * *************************************************************************/

#ifndef SOUND_H
#define SOUND_H
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

class Sound{
public:
    Sound();
    Mix_Music *mLand, *mAir, *mNaval,
    *attack[9], *destroy, *capture;
};

#endif // SOUND_H

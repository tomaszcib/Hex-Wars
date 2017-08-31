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
#include <QCoreApplication>
#include <QString>
#include <QByteArray>

Sound::Sound(){
    QString str1 = QCoreApplication::applicationDirPath() + "/audio/";

    QByteArray ba = (str1 + "/foot3.ogg").toLatin1();
    const char *c_str2 = ba.data();
    mLand = Mix_LoadMUS(c_str2);

    ba = (str1 + "/air4.ogg").toLatin1();
    c_str2 = ba.data();
    mAir = Mix_LoadMUS(c_str2);

    ba = (str1 + "/inh2o.ogg").toLatin1();
    c_str2 = ba.data();
    mNaval = Mix_LoadMUS(c_str2);

    ba = (str1 + "/MgBar1.ogg").toLatin1();
    c_str2 = ba.data();
    attack[0] = Mix_LoadMUS(c_str2);
    attack[1] = NULL;

    ba = (str1 + "/MgBar2.ogg").toLatin1();
    c_str2 = ba.data();
    attack[2] = Mix_LoadMUS(c_str2);
    attack[3] = NULL;
    attack[4] = NULL;

    ba = (str1 + "/LrgCan.ogg").toLatin1();
    c_str2 = ba.data();
    attack[6] = Mix_LoadMUS(c_str2);

    ba = (str1 + "/MedCan.ogg").toLatin1();
    c_str2 = ba.data();
    attack[5] = Mix_LoadMUS(c_str2);
    attack[7] = Mix_LoadMUS(c_str2);
    attack[8] = Mix_LoadMUS(c_str2);

    ba = (str1 + "/SmlExpl.ogg").toLatin1();
    c_str2 = ba.data();
    destroy = Mix_LoadMUS(c_str2);

    ba = (str1 + "/wall01.ogg").toLatin1();
    c_str2 = ba.data();
    capture = Mix_LoadMUS(c_str2);
}

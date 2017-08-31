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
 *  /ui/graphics.cpp
 *
 *  all sort of graphical functions: extracting images from included texture sets,
 *  painting hexes and units onto the board, creating and painting the left-dock
 *  minimap.
 *
 * *************************************************************************/

#include "../mainwind.h"

/* Sum of 2 QImages of the same size;
 * we always place QImage b on QImage a,
 * so sum(a,b) does not equal sum(b,a) */
QImage MainWind::sumImages(QImage *a, QImage *b){
    int i, j;
    QImage res = *a;
    for(i = 0; i < b->height(); i++)
        for(j = 0; j < b->width(); j++)
            if((b->pixel(j, i) & 0x00ffffff) != 0x00000000)
                res.setPixel(j, i, b->pixel(j, i));
    return res;
}

/* Paint an unit on a hex */
QImage MainWind::paintUnit(QImage *hexT, QImage *unitT){
    int i, j;
    QImage res = *hexT;
    /* First half is shifted to right */
    for(i = 0; i < UNIT_SIZE; i++)
        for(j = 0; j < UNIT_SIZE/2; j++)
            res.setPixel(28 + j, 9 + i, unitT->pixel(j, i));
    /* Second half is shifted to left */
    for(i = 0; i < UNIT_SIZE; i++)
        for(j = 0; j < UNIT_SIZE/2; j++)
            res.setPixel(j, 9 + i, unitT->pixel(UNIT_SIZE/2 + j, i));
    return res;
}

QImage MainWind::paintMoreUnits(QImage *hexT, QImage *digit){
    int i, j;
    QImage res = *hexT;
    for(i = 0; i < 13; i++)
        for(j = 0; j < 9; j++)
            res.setPixel(j + 6, 9 + i, digit->pixel(j, i));
    return res;
}

QImage MainWind::paintTransported(QImage *hexT, QImage *digit){
    int i, j;
    QImage res = *hexT;
    for(i = 0; i < 13; i++)
        for(j = 0; j < 9; j++)
            res.setPixel(28 + j, 9 + i, digit->pixel(j, i));
    return res;
}

/* Paints health bar at the bottom of the unit */
QImage MainWind::paintHPBar(QImage *hexT, int health){
    QRgb color;
    int i;
    /* Green - high health, orange - medium, red - low */
    if(health >= 67) color = 0xff00aa00;
    else if(health >= 33 && health < 67) color = 0xffff8000;
    else color = 0xffaa0000;
    health = ceil(health * 0.3);
    QImage res = *hexT;
    for(i = 0; i < health - 15; i++){
        res.setPixel(i, 36, color);
        res.setPixel(i, 37, color);
        res.setPixel(i, 38, color);
    }
    health = (health >= 15 ? 15 : health);
    for(i = 0; i < health; i++){
        res.setPixel(28 + i, 36, color);
        res.setPixel(28 + i, 37, color);
        res.setPixel(28 + i, 38, color);
    }
    return res;
}

void MainWind::createTextures(){
    QImage allTextures;

    allTextures = QImage(":/images/textureHex.png", "PNG");
    texture = new QImage[NOF_TEXTURES];
    for(int i = 0; i < NOF_TEXTURES; i++){
        texture[i] = allTextures.copy( (i%11) * TEXTURE_WIDTH + (i%11 + 1), (i/11) * TEXTURE_HEIGHT + (i/11 +1 ), TEXTURE_WIDTH, TEXTURE_HEIGHT);
        texture[i] = texture[i].convertToFormat(QImage::Format_RGB16);
    }

    /* Now we load unit icons */
    allTextures = QImage(":/images/units.png", "PNG");
    unitImage = new QImage[72];
    for(int i = 0; i < 72; i++){
        unitImage[i] = allTextures.copy( (i%9) * UNIT_SIZE + (i%9 + 1), (i/9) * UNIT_SIZE + (i/9 + 1), UNIT_SIZE, UNIT_SIZE);
        unitImage[i] = unitImage[i].convertToFormat(QImage::Format_RGB16);
    }

    /* Create graphical digits */
    allTextures = QImage(":/images/digits.png", "PNG");
    digit = new QImage[9];
    for(int i = 0; i < 9; i++){
        digit[i] = allTextures.copy(i * 9 + i, 0, 9, 13);
        digit[i] = digit[i].convertToFormat(QImage::Format_Mono);
    }
}

void MainWind::paintHex(int i){
    u_int8_t isVisible;
    isVisible = !isHexVisible(i, lastHumanPlayer);

    if(isHexUncovered(i, lastHumanPlayer)){

        /* Paint water hex - we may add shores to it if it's contiguous to land hexes */
        if(hexagon[i].height <= WATER_LEVEL){
            hexagon[i].texture = texture[TEXTURE_WATER + isVisible * 11];
            for(int k = 0; k < hexagon[i].contig.size(); k++){
                if(hexagon[hexagon[i].contig[k]].height > WATER_LEVEL)
                        hexagon[i].texture = sumImages(&hexagon[i].texture, &texture[hexWhichDirection(i, hexagon[i].contig[k]) + 3 + isVisible * 11]);
            }
        }        

        /* Paint land hex */
        else {
            hexagon[i].texture = texture[TEXTURE_GRASS + isVisible * 11];
            if(hexagon[i].height > NOACCESS_LEVEL)
                hexagon[i].texture = sumImages(&hexagon[i].texture, &texture[TEXTURE_MOUNT + isVisible * 11]);
            else if(hexagon[i].terrain == 1){
                hexagon[i].texture = paintUnit(&hexagon[i].texture, &unitImage[54 + city[hexagon[i].cityId].owner + isVisible * 9]);
                if(city[hexagon[i].cityId].owner == lastHumanPlayer && !hexagon[i].unit.empty())
                    hexagon[i].texture = paintMoreUnits(&hexagon[i].texture, &digit[hexagon[i].unit.size() - 1]);
            }
        }

        /* Paint unit on hex */
        if(!isVisible && !hexagon[i].unit.empty() && !(hexagon[i].terrain == 1 && i != curHex)){
            if(i == curHex){
                for(int j = 0; j < 43; j++)
                    for(int k = 0; k < 48; k++)
                        if(!((j + k * 43) % 7) || !((j + 1 + k * 43) % 7))
                            hexagon[i].texture.setPixel(j, k, 0xffffff00);
            }
            hexagon[i].texture = paintUnit(&hexagon[i].texture, &unitImage[hexagon[i].unit[0].type + hexagon[i].unit[0].owner * 9]);
            if(hexagon[i].unit[0].owner == lastHumanPlayer){
                if(hexagon[i].unit.size() > 1 && (hexagon[i].unit[0].type == 4 || hexagon[i].unit[0].type == 7))
                    hexagon[i].texture = paintMoreUnits(&hexagon[i].texture, &digit[hexagon[i].unit.size() - 2]);
                hexagon[i].texture = paintHPBar(&hexagon[i].texture, hexagon[i].unit[0].HP);
            }
        }

    }
    else hexagon[i].texture = texture[2];

    /* Compress the texture */
    hexagon[i].texture = hexagon[i].texture.convertToFormat(QImage::Format_RGB444);
    drawMinimapHex(i);
}

void MainWind::createMinimap(){
    minimap.squareSize = 200 / rows;
    minimap.pixelScale = 200 / (rows * sizeInHexes(1));
    minimap.image = QImage(minimap.squareSize * rows, minimap.squareSize * cols, QImage::Format_RGB16);
}

void MainWind::drawMinimapHex(int hex){
    int shiftX, shiftY, color, j, k;
    shiftX = hex % rows * minimap.squareSize;
    shiftY = hex / rows * minimap.squareSize;
    if(!isHexUncovered(hex, lastHumanPlayer)) color = 0xff000000;
    else{
        if(hexagon[hex].height > WATER_LEVEL) color = 0xff008800;
        else color = 0xff000080;
        for(int i = 0; i < hexagon[hex].contig.size(); i++)
            if(city[hexagon[hex].region].owner != city[hexagon[hexagon[hex].contig[i]].region].owner
                    && city[hexagon[hex].region].owner != 6)
                color = player[city[hexagon[hex].region].owner].color;
    }
    for(j = shiftX; j < shiftX + minimap.squareSize; j++)
        for(k = shiftY; k < shiftY + minimap.squareSize; k++){
            minimap.image.setPixel(j, k, color);
            if(!((j + k) % 5) && (city[hexagon[hex].region].owner != 6) && isHexUncovered(hex, lastHumanPlayer))
                minimap.image.setPixel(j, k, player[city[hexagon[hex].region].owner].color);
        }
}

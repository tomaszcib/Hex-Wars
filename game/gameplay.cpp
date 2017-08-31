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
 *  /game/gameplay.cpp
 *
 *  functions essential for gameplay - switching players, turn begin/end
 *  routines and game ending when conditions are met
 *
 * *************************************************************************/

#include "../mainwind.h"

/* This function is executed when human player either starts a new game,
 * ends his/her turn or surrenders. Basically, it iterates through players
 * until another human player is active or there are no other players left */
void MainWind::endTurn(){
    do{
        resetDiscoveredMap();
        curPlayer = (curPlayer + 1) % 6;
        if(!curPlayer){
            curTurn++;
            createMainWindTitle();
        }
        updateNowPlaying();
        delay();
        beginTurn();
        if(playersInGame == 1 || !humansInGame) {
            endGame(playersInGame == 1 ? 0 : 1);
            return;
        }
        /* This is an AI player. It's its turn now */
        if(!player[curPlayer].isHuman && player[curPlayer].isActive)
            compMove();
    } while(!(player[curPlayer].isHuman && player[curPlayer].isActive));
    if(option[2]->isChecked()){
        ingameInfo->setWindowTitle(str.string[16]);
        ingameInfo->setText(str.string[129]);
        ingameInfo->setIconPixmap(QPixmap::fromImage(unitImage[curPlayer * 9]));
        ingameInfo->exec();
    }
    lastHumanPlayer = curPlayer;
    panelAction[6]->setIcon(QPixmap::fromImage(unitImage[54 + lastHumanPlayer]));
    panelAction[7]->setDisabled(false);
    menuAction[1]->setDisabled(false);
    menuAction[2]->setDisabled(false);
    menuAction[10]->setDisabled(false);
    for(int i = 0; i < SIZEOF_MAP; i++)
        paintHex(i);
    updateMinimap();
    update();
}

/* Cover all hexes outside curPlayer's zone of control */
void MainWind::resetDiscoveredMap(){
    for(int i = 0; i < SIZEOF_MAP; i++)
        setHexInvisible(i, curPlayer);
    for(int i = 0; i < SIZEOF_MAP; i++)
        if(isContainingAlly(i, curPlayer)){
            if(hexagon[i].terrain == 1) unitAppear(i, 0, curPlayer);
            else if(!hexagon[i].unit.empty())
                unitAppear(i, (hexagon[i].unit[0].type < 2) ? 1 : 0, curPlayer);
        }
}

/* Preparations done before one's turn */
void MainWind::beginTurn(){
    int cityId, u, type;
    /* If player has no cities and units, mark him/her as inactive */
    if(!player[curPlayer].citiesControl && !player[curPlayer].unitsControl
            && player[curPlayer].isActive){
        LOG(GAME, str.log[14 + curPlayer] + str.log[20]);
        player[curPlayer].isActive = false;
        ingameInfo->setWindowTitle(str.string[66]);
        ingameInfo->setText(str.string[65]);
        ingameInfo->setIconPixmap(QPixmap::fromImage(unitImage[curPlayer * 9]));
        ingameInfo->exec();
        /* If it was human player - check if we can qualify for a new highscore */
        if(player[curPlayer].isHuman){
            humansInGame--;
            highScoreSave(curPlayer);
        }
        playersInGame--;
    }
    /* If player isn't out, begin his/her turn */
    else {
        for(int i = 0; i < SIZEOF_MAP; i++){
            hexagon[i].vis = 0;
            if(isContainingAlly(i, curPlayer) && !hexagon[i].unit.empty())
                for(u = 0; u < hexagon[i].unit.size(); u++){
                    type = hexagon[i].unit[u].type;
                    /* If it's air unit, replenish its fuel if it's on city or carrier.
                     * If fuel drops to zero, destroy the unit */
                    if(type >= PARAS && type <= BOMBER){
                        if(hasFriendlyShip(i, curPlayer, CARRIER) || hasFriendlyCity(i, curPlayer))
                            hexagon[i].unit[u].fuel = rule.maxFuel[type];
                        else if(!(--hexagon[i].unit[u].fuel)){
                            if(type == PARAS && canLandParas(i))
                                landParas(i);
                            else destroyUnit(i, curPlayer);
                            continue;
                        }
                    }
                    /* If it's naval unit and it's damaged and it's near friendly city,
                     * repair this unit a little bit */
                    else if(type >= 4 && isContigToFriendlyCity(i, curPlayer)
                            && hexagon[i].unit[u].HP < 100){
                        hexagon[i].unit[u].HP += 25;
                        if(hexagon[i].unit[u].HP > 100)
                            hexagon[i].unit[u].HP = 100;
                    }
                    hexagon[i].unit[u].moves = rule.maxMove[type];
                    if(type >= 4){
                        hexagon[i].unit[u].moves *= (float)(hexagon[i].unit[u].HP / 100.0);
                        if(!hexagon[i].unit[u].moves)
                            hexagon[i].unit[u].moves = 1;
                    }
                }
            if(hexagon[i].terrain == 1 && city[hexagon[i].cityId].owner == curPlayer
                    && city[hexagon[i].cityId].produces != NOPRODUCTION){
                cityId = hexagon[i].cityId;
                if(!(--city[cityId].timeToProduce)){
                    spawnUnit(i, newUnit(city[cityId].produces, curPlayer));
                    if(player[curPlayer].isHuman)
                        city[cityId].timeToProduce = rule.prodTime[city[cityId].produces];
                    else {
                        city[cityId].timeToProduce = 255;
                        city[cityId].produces = NOPRODUCTION;
                    }
                }
            }
        }
    }
}

void MainWind::endGame(int how){
    u_int8_t i = 0;
    ingameInfo->setWindowTitle(str.string[64]);
     /* Case 0 - here's only 1 player left, he/she wins the game */
    if(how == 0){
        ingameInfo->setText(str.string[62]);
        for(i = 0; i < 6; i++){
            if(player[i].isActive){
                LOG(GAME, str.log[21] + str.log[14 + i] + " " + str.string[62]);
                ingameInfo->setIconPixmap(QPixmap::fromImage(unitImage[i * 9]));
                break;
            }
        }
    }
    /* Case 1 - there are no human players left */
    else if(how == 1){
        LOG(GAME, str.log[21] + str.string[63]);
        ingameInfo->setText(str.string[63]);
        ingameInfo->setIcon(QMessageBox::Information);
    }
    menuAction[1]->setDisabled(false);
    menuAction[2]->setDisabled(true);
    menuAction[10]->setDisabled(true);
    panelAction[7]->setDisabled(true);
    ingameInfo->exec();
    if(how == 0 && player[i].isHuman)
        highScoreSave(i);
    LOG(GAME, str.log[22]);
    lastHumanPlayer = 7;
    selectNothingEffect();
    for(int i = 0; i < SIZEOF_MAP; i++)
        paintHex(i);
    updateMinimap();
    update();
    updateNowPlaying();

}

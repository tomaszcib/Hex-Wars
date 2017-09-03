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
 *  /ui/actions.cpp
 *
 *  functions connected to in-game widgets, such as push buttons, dialog windows,
 *  menu actions, this includes routines for starting a new game from "New game"
 *  window and loading or saving games.
 *
 *  NOTE: these functions are mostly private slots of MainWind class.
 *  In this source's convention all slots have names with 'do' prefices
 *  (ex. doNextUnit)
 *
 * *************************************************************************/

#include "../mainwind.h"

/* Update main map hex painter - check if we have to paint grid */
void MainWind::doUpdateBorderPen(){
    if(option[1]->isChecked()) borderPen = QPen(Qt::black);
    else borderPen = QPen(Qt::transparent);
}

/* Actions on left-side dock */
void MainWind::doNextUnit(){
    hexagon[curHex].unit.append(hexagon[curHex].unit[0]);
    hexagon[curHex].unit.pop_front();
    updateSelection();
    paintHex(curHex);
    update();
}

void MainWind::doPrevUnit(){
    hexagon[curHex].unit.push_front(hexagon[curHex].unit[hexagon[curHex].unit.size() - 1]);
    hexagon[curHex].unit.pop_back();
    updateSelection();
    paintHex(curHex);
    update();
}

void MainWind::doDeselect(){
    selectNothingEffect();
}

void MainWind::doLandParas(){
    landParas(curHex);
    selectNothingEffect();
    update();
}

void MainWind::doCenterCameraAt(){
    moveCameraTo(curHex);
}

void MainWind::doDelete(){
    ingameQuestion.setWindowTitle(str.string[11]);
    ingameQuestion.setText(str.string[121]);
    if(ingameQuestion.exec() == QMessageBox::Yes){
        int oldHex = curHex;
        destroyUnit(curHex, curPlayer);
        selectNothingEffect();
        curHex = oldHex;
        if(!hexagon[curHex].unit.empty())
            selectUnitEffect();
        update();
    }
    else ingameQuestion.close();
}

/* Ask if a player wants to surrender */
void MainWind::doSurrender(){
    ingameQuestion.setWindowTitle(str.string[80]);
    ingameQuestion.setText(str.string[81]);
    if(ingameQuestion.exec() == QMessageBox::Yes){
        selectNothingEffect();
        panelAction[7]->setDisabled(true);
        menuAction[2]->setDisabled(true);
        menuAction[10]->setDisabled(true);
        for(int i = 0; i < SIZEOF_MAP; i++){
            if(isContainingAlly(i, lastHumanPlayer) && !hexagon[i].unit.empty())
                hexagon[i].unit.clear();
            if(hasFriendlyCity(i, lastHumanPlayer))
                city[hexagon[i].cityId].owner = 6;
        }
        player[lastHumanPlayer].unitsControl = 0;
        player[lastHumanPlayer].citiesControl = 0;
        drawDockChart();
        endTurn();
    }
    else ingameQuestion.close();
}

void MainWind::doCityCloseUp(){
    curCity = hexagon[curHex].cityId;
    cityWindow.prodOpt[city[curCity].produces]->setChecked(true);
    cityWindow.setWindowIcon(QPixmap::fromImage(unitImage[54 + lastHumanPlayer]));
    doUpdateProdLabel();
    cityWindow.show();
}

void MainWind::doEndTurn(){
    selectNothingEffect();
    panelAction[7]->setDisabled(true);
    menuAction[1]->setDisabled(true);
    menuAction[2]->setDisabled(true);
    endTurn();
}

/* Actions on Goto Hex window */
void MainWind::doShowGoto(){
    gotoWindow.show();
}

void MainWind::doGotoHex(){
    moveCameraTo((gotoWindow.spin[0]->value() - 1) * rows
            + (gotoWindow.spin[1]->value() -1));
    gotoWindow.close();
}

/* Actions on city close-up window */
void MainWind::doUpdateProdLabel(){
    int prod = 0;
    for(int i = 0; i < 9; i++){
        if(cityWindow.prodOpt[i]->isChecked()){
            prod = i;
            break;
        }
    }
    cityWindow.curProdIcon->setPixmap(QPixmap::fromImage(unitImage[curPlayer * 9 + prod]));
    cityWindow.curProd->setText(str.string[14] +
            QString::number(prod == city[curCity].produces ? city[curCity].timeToProduce : rule.prodTime[prod])
            + str.string[15]);
}

void MainWind::doSetCityProduction(){
    for(int i = 0; i < 9; i++){
        if(cityWindow.prodOpt[i]->isChecked()){
            city[curCity].timeToProduce = (i == city[curCity].produces ? city[curCity].timeToProduce : rule.prodTime[i]);
            city[curCity].produces = i;
            cityWindow.close();
            return;
        }
    }
    cityWindow.close();
}

/* Actions on piece rules edition window */
void MainWind::doSetDefaultRules(){
    tmpRule.setDefaultRules();
    for(int i = 0; i < 9; i++){
        rulesWindow.prod[i]->setText(QString::number(tmpRule.prodTime[i]));
        rulesWindow.moves[i]->setText(QString::number(tmpRule.maxMove[i]));
        if(i >= 1 && i <= 3 )
            rulesWindow.fuel[i]->setText(QString::number(tmpRule.maxFuel[i]));
    }
}

void MainWind::doUpdateNewGameRules(){
    for(int i = 0; i < 9; i++){
        tmpRule.prodTime[i] = (rulesWindow.prod[i]->text()).toInt();
        tmpRule.maxMove[i] = (rulesWindow.moves[i]->text()).toInt();
        if(i >= 1 && i <= 3)
            tmpRule.maxFuel[i] = (rulesWindow.fuel[i]->text()).toInt();
    }
    rulesWindow.close();
}

/* Actions on new game window */
void MainWind::doShowNewGameWindow(){
    newGameWindow.seed->setValue(R.rand()%16777216);
    newGameWindow.genOpt[0]->setCurrentIndex(2);
    newGameWindow.genOpt[1]->setCurrentIndex(1);
    newGameWindow.genOpt[2]->setCurrentIndex(0);
    for(int i = 1; i < 6; i++)
        newGameWindow.playerOpt[i]->setCurrentIndex(3);
    newGameWindow.playerOpt[0]->setCurrentIndex(1);
    doSetDefaultRules();
    newGameWindow.show();
}

/* Recalculate difficulty when user changes players settings
 * in new game window */
void MainWind::doUpdateDiffLabel(){
    u_int8_t diff = 0, i, humans = 0, players = 0, val;
    for(i = 0; i < 6; i++){
        val = newGameWindow.playerOpt[i]->currentIndex();
        if(val) players++;
        if(val == 1) { if(++humans > 1) { diff = 255; break; } }
        else if(val == 2) diff += 5;
        else if(val == 3) diff += 10;
        else if(val == 4) diff += 20;
    }
    if(!humans || players < 2){
        newGameWindow.ok->setDisabled(true);
        newGameWindow.difficulty->setText(!humans ? str.string[100] : str.string[101]);
    }
    else{
        newGameWindow.ok->setDisabled(false);
        newGameWindow.difficulty->setText(str.string[98] + ": " + (diff == 255 ? str.string[99] :
                                          (QString::number(diff)+"%")));
    }
}

void MainWind::doShowRulesEdit(){
    rulesWindow.show();
}

/* Create a new game - within "New Game" window */
void MainWind::doNewGame(){
    int size, landAmount;
    LOG(INFO, str.log[7]);
    /* Clear the map */
    selectNothingEffect();
    for(int i = 0; i < SIZEOF_MAP; i++){
        hexagon[i].unit.clear();
        hexagon[i].contig.clear();
        hexagon[i].texture = QImage();
    }
    curTurn = 0;
    SIZEOF_MAP = 0;
    shapeItems.clear();
    city.clear();
    /* Read player options */
    humansInGame = 0;
    playersInGame = 0;
    for(int i = 0; i < 6; i++){
        player[i].citiesControl = 0;
        player[i].unitsControl = 0;
        player[i].scoreKilled = 0;
        player[i].scoreProduced = 0;
        player[i].level = newGameWindow.playerOpt[i]->currentIndex();
        if(newGameWindow.playerOpt[i]->currentIndex() != 0){
            player[i].isActive = true;
            playersInGame++;
        }
        else player[i].isActive = false;
        if(newGameWindow.playerOpt[i]->currentIndex() == 1){
            player[i].isHuman = true;
            humansInGame++;
        }
        else player[i].isHuman = false;
    }
    /* Calculate difficulty */
    gdiff = 0;
    if(humansInGame > 1) gdiff = 255;
    else{
        for(int i = 0; i < 6; i++){
            if(player[i].level == 2) gdiff += 5;
            else if(player[i].level == 3) gdiff += 10;
            else if(player[i].level == 4) gdiff += 20;
        }
    }
    /* Set piece rules */
    rule = tmpRule;

    /* Read world options */
    size = MIN_MAPSIZE + 5 * newGameWindow.genOpt[0]->currentIndex();
    landAmount = size;
    if(newGameWindow.genOpt[1]->currentIndex() == 0) landAmount *= 0.75;
    else if(newGameWindow.genOpt[1]->currentIndex() == 2) landAmount *= 1.25;
    tAmount = landAmount;


    /* Apply seed */
    R.srand(newGameWindow.seed->value());
    gseed = newGameWindow.seed->value();

    /* Generate the world */
    generateIsland(size, landAmount);
    if(newGameWindow.genOpt[2]->currentIndex() == 0)
        R.srand(QTime(0,0,0).secsTo(QTime::currentTime()));
    spawnStartingPos();
    drawDockChart();
    updateMinimap();

    /* Finish the procedure, set global variables */
    R.srand(QTime(0,0,0).secsTo(QTime::currentTime()));
    curPlayer = 5;
    newGameWindow.close();
    countPlayerUnits();
    moveCameraTo((SIZEOF_MAP + rows) / 2);
    endTurn();
    updateMinimapPointer();
    gotoWindow.spin[0]->setRange(1, rows);
    gotoWindow.spin[1]->setRange(1, cols);
    menuAction[7]->setDisabled(false);
    menuAction[11]->setDisabled(false);
    LOG(INFO, str.log[13]);
}

void MainWind::doShowGameInfo(){
    ingameInfo->setWindowTitle(str.string[67]);
    ingameInfo->setText(str.string[68]);
    ingameInfo->setIconPixmap(QPixmap::fromImage(QImage(":/images/HexWarsIcon.png", "PNG")));
    ingameInfo->exec();
}

/* Trigger closeEvent of the main window */
void MainWind::doClose(){
    this->close();
}

/* Open the program's webpage in default browser */
void MainWind::doVisitWWW(){
    QDesktopServices::openUrl(QUrl("http://www-users.mat.umk.pl/~tomaszcib/"));
}

/* Open the game's manual in default browser */
void MainWind::doManual(){
    QDesktopServices::openUrl(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/doc/help.html"));
}

/* Show rankings window for the current game */
void MainWind::doShowRankings(){
    u_int8_t i, table[6];

    /* City ranking */
    createRanking(1, table);
    for(i = 0; i < 6; i++)
        rankingWindow.item[(i+1)*4+1]->
                setText(player[i].isActive ? str.string[table[i] + 82] : "-");
    /* Unit ranking */
    createRanking(0, table);
    for(i = 0; i < 6; i++)
        rankingWindow.item[(i+1)*4+2]->
                setText(player[i].isActive ? str.string[table[i] + 82] : "-");
    /* Print player types and scores */
    for(i = 0; i < 6; i++){
        rankingWindow.item[(i+1)*4]->setText(str.string[player[i].level + 40]);
        rankingWindow.item[(i+1)*4+3]->setText(QString::number(player[i].scoreKilled / 3
                                               + player[i].scoreProduced / 10));
    }
    rankingWindow.show();
}

/* Show general highscores window */
void MainWind::doShowHighScores(){
    highscoreWindow.show();
}

/* Show the game's log. */
void MainWind::doShowLog(){
    logWindow.show();
}


/* Loading game procedure */
void MainWind::doLoadGame(){
    Unit u;

    QList<QString> fileName;
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setWindowTitle(str.string[55]);
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    if(fileDialog->exec())
        fileName = fileDialog->selectedFiles();
    if(fileName.empty()) return;
    QFile file(fileName[0]);
    if(file.open(QIODevice::ReadOnly)){
        QDataStream in(&file);
        LOG(INFO, str.log[3]);
        /* Clear the existing data first */
        selectNothingEffect();
        for(int i = 0; i < SIZEOF_MAP; i++){
            hexagon[i].unit.clear();
            hexagon[i].contig.clear();
            hexagon[i].texture = QImage();
        }
        SIZEOF_MAP = 0;
        shapeItems.clear();
        city.clear();

        /* Load the data, according to the .SAV specification */
        quint8 t8[6];           //uint8 buffer
        quint32 t32[2];         //uint32 buffer
        in >> t8[0];
        /* Incompatible version of the save file */
        if(t8[0] != GAME_VERSION){
            ingameInfo->setWindowTitle(str.string[102]);
            ingameInfo->setWindowTitle(str.string[103]);
            ingameInfo->setIcon(QMessageBox::Warning);
            ingameInfo->exec();
            return;
        }
        in >> t32[0] >> t32[1] >> t8[0] >> t8[1] >> t8[2] >> t8[3] >> t8[4]
            >> t8[5];
        gseed = t32[0];
        R.srand(gseed);
        curTurn = t32[1];
        cols = t8[0];
        tAmount = t8[1];
        curPlayer = t8[2];
        lastHumanPlayer = t8[3];
        humansInGame = t8[4];
        playersInGame = t8[5];
        in >> t8[0];
        gdiff = t8[0];
        generateIsland(cols, tAmount);

        for(int i = 0; i < 6; i++){
            in >> t32[0] >> t32[1];
            player[i].unitsControl = t32[0];
            player[i].citiesControl = t32[1];
            in >> t32[0] >> t32[1];
            player[i].scoreKilled = t32[0];
            player[i].scoreProduced = t32[1];
            in >> t8[0] >> t8[1] >> t8[2];
            player[i].isHuman = t8[0];
            player[i].isActive = t8[1];
            player[i].level = t8[2];
        }
        for(int i = 0; i < SIZEOF_MAP; i++){
            in >> t8[0];
            for(int j = 0; j < t8[0]; j++){
                in >> t8[1] >> t8[2] >> t8[3] >> t8[4] >> t8[5];
                u.type = t8[1];
                u.owner = t8[2];
                u.HP = t8[3];
                u.moves = t8[4];
                u.fuel = t8[5];
                if(!player[u.owner].isHuman){
                    in >> t32[0] >> t8[1];
                    u.gotoHex = t32[0];
                    u.mode = t8[1];
                }
                if(u.type == TANK && !player[u.owner].isHuman){
                    in >> t32[0] >> t32[1] >> t8[1];
                    u.transpReq.to = t32[0];
                    u.transpReq.urgency = t32[1];
                    u.transpReq.waitTime = t8[1];
                    in >> t32[0];
                    u.transpReq.where = t32[0];
                }
                hexagon[i].unit.append(u);
            }
            in >> t8[0] >> t8[1] >> t8[2];
            hexagon[i].vis = t8[0];
            hexagon[i].discovered = t8[1];
            hexagon[i].visible = t8[2];
        }
        for(int i = 0; i < city.size(); i++){
            in >> t8[0] >> t8[1] >> t32[0] >> t8[2];
            city[i].produces = t8[0];
            city[i].owner = t8[1];
            city[i].pos = t32[0];
            city[i].timeToProduce = t8[2];
        }
        for(int i = 0; i < 9; i++){
            in >> t8[0] >> t8[1] >> t8[2];
            rule.prodTime[i] = t8[0];
            rule.maxMove[i] = t8[1];
            rule.maxFuel[i] = t8[2];
        }
        file.close();

        panelAction[7]->setDisabled(false);
        menuAction[2]->setDisabled(false);
        panelAction[6]->setIcon(QPixmap::fromImage(unitImage[54 + lastHumanPlayer]));
        selectNothingEffect();
        drawDockChart();
        updateMinimap();
        for(int i = 0; i < SIZEOF_MAP; i++)
            paintHex(i);
        updateMinimap();
        moveCameraTo((SIZEOF_MAP + rows) / 2);
        updateNowPlaying();
        createMainWindTitle();
        countPlayerUnits();
        menuAction[7]->setDisabled(false);
        menuAction[11]->setDisabled(false);
        gotoWindow.spin[0]->setRange(1, rows);
        gotoWindow.spin[1]->setRange(1, cols);
        LOG(INFO, str.log[4]);
    }
    else {
        LOG(ERROR, str.string[74] + "'" + fileName[0] + "'");
        ingameInfo->setWindowTitle(str.string[73]);
        ingameInfo->setText(str.string[74] + "'" + fileName[0] + "'");
        ingameInfo->setIcon(QMessageBox::Critical);
        ingameInfo->exec();
    }

}

/* Saving the game into a file */
void MainWind::doSaveGame(){
    Unit u;

    /* Set the QFileDialog window to proper workmode */
    QList<QString> fileName;
    fileDialog->setFileMode(QFileDialog::AnyFile);
    fileDialog->setWindowTitle(str.string[56]);
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    if(fileDialog->exec())
        fileName = fileDialog->selectedFiles();
    if(fileName.empty()) return;
    QFile file(fileName[0]);
    if(file.exists()){
        ingameQuestion.setWindowTitle(str.string[78]);
        ingameQuestion.setText("'" + fileName[0]+ "'" + str.string[79]);
        if(ingameQuestion.exec() == QMessageBox::Yes) goto perform;
        else return;
    }
    perform:
    if(file.open(QIODevice::WriteOnly)){
        QDataStream out(&file);
        LOG(INFO, str.log[5]);
        /* Save the data, according to the .SAV specification */
        out << (quint8)GAME_VERSION
            << (quint32)gseed
            << (quint32)curTurn
            << (quint8)cols
            << (quint8)tAmount
            << (quint8)curPlayer
            << (quint8)lastHumanPlayer
            << (quint8)humansInGame
            << (quint8)playersInGame
            << (quint8)gdiff;
        for(int i = 0; i < 6; i++){
            out << (quint32)player[i].unitsControl
                << (quint32)player[i].citiesControl
                << (quint32)player[i].scoreKilled
                << (quint32)player[i].scoreProduced
                << (quint8)player[i].isHuman
                << (quint8)player[i].isActive
                << (quint8)player[i].level;
        }
        for(int i = 0; i < SIZEOF_MAP; i++){
            out << (quint8)hexagon[i].unit.size();
            foreach(u, hexagon[i].unit){
                out << (quint8)u.type
                    << (quint8)u.owner
                    << (quint8)u.HP
                    << (quint8)u.moves
                    << (quint8)u.fuel;
                if(!player[u.owner].isHuman)
                    out << (quint32)u.gotoHex
                        << (quint8)u.mode;
                if(u.type == TANK && !player[u.owner].isHuman)
                    out <<(quint32)u.transpReq.to
                        <<(quint32)u.transpReq.urgency
                        <<(quint8)u.transpReq.waitTime
                        <<(quint32)u.transpReq.where;
            }
            out << (quint8)hexagon[i].vis
                << (quint8)hexagon[i].discovered
                << (quint8)hexagon[i].visible;
        }
        for(int i = 0; i < city.size(); i++){
            out << (quint8)city[i].produces
                << (quint8)city[i].owner
                << (quint32)city[i].pos
                << (quint8)city[i].timeToProduce;
        }
        for(int i = 0; i < 9; i++){
            out << (quint8)rule.prodTime[i]
                << (quint8)rule.maxMove[i]
                << (quint8)rule.maxFuel[i];
        }

        /* Close the file */
        file.close();
        LOG(INFO, str.log[6]);
    }
    else {
        LOG(ERROR, str.string[75] + "'" + fileName[0] + "'");
        ingameInfo->setWindowTitle(str.string[73]);
        ingameInfo->setText(str.string[75] + "'" + fileName[0]+ "'");
        ingameInfo->setIcon(QMessageBox::Critical);
        ingameInfo->exec();
    }
}

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
 *  /ui/miscui.cpp
 *
 *  miscellaneous GUI elements - MainWind menus and dialog windows setup.
 *
 * *************************************************************************/

#include "../mainwind.h"

/* Create windows */
void MainWind::createWindows(){
    /* New game window */
    newGameWindow.setWindowTitle(str.string[28]);
    for(int i = 0; i < 6; i++){
        if(i < 4)
            newGameWindow.generatorLabel[i]->setText(str.string[31 + i]);
        newGameWindow.playerLabel[i]->setPixmap(QPixmap::fromImage(unitImage[i * 9]).scaled(20, 20));
        connect(newGameWindow.playerOpt[i], SIGNAL(currentIndexChanged(int)), this, SLOT(doUpdateDiffLabel()));
        for(int j = 0; j < 5; j++)
            newGameWindow.playerOpt[i]->addItem(str.string[40 + j]);
    }
    for(int i = MIN_MAPSIZE; i <= MAX_MAPSIZE; i += 5)
        newGameWindow.genOpt[0]->addItem(QString::number(i) + "x" + QString::number(i));

    for(int i = 0; i < 3; i++){
        if(i < 2)
            newGameWindow.genOpt[2]->addItem(str.string[i + 38]);
        newGameWindow.genOpt[1]->addItem(str.string[i + 35]);
    }
    newGameWindow.ok->setText(str.string[50]);
    newGameWindow.cancel->setText(str.string[51]);
    newGameWindow.editRules->setText(str.string[46]);
    newGameWindow.genGroup->setTitle(str.string[29]);
    newGameWindow.playerGroup->setTitle(str.string[30]);
    connect(newGameWindow.editRules, SIGNAL(released()), this, SLOT(doShowRulesEdit()));
    connect(newGameWindow.ok, SIGNAL(released()), this, SLOT(doNewGame()));

    /* Edit rules window */
    rulesWindow.setWindowTitle(str.string[45]);
    for(int i = 0; i < 9; i++){
        if(i >= 1 && i <= 3) rulesWindow.title[i]->setText(str.string[46 + i]);
        rulesWindow.unitName[i]->setText(str.string[17 + i]);
    }
    rulesWindow.title[0]->setText("");
    rulesWindow.setModal(true);
    rulesWindow.ok->setText(str.string[50]);
    rulesWindow.cancel->setText(str.string[51]);
    rulesWindow.defaults->setText(str.string[72]);
    connect(rulesWindow.ok, SIGNAL(released()), this, SLOT(doUpdateNewGameRules()));
    connect(rulesWindow.defaults, SIGNAL(released()), this, SLOT(doSetDefaultRules()));

    /* Set production window */
    cityWindow.setWindowTitle(str.string[27]);
    cityWindow.prodSelect->setTitle(str.string[26]);
    cityWindow.setModal(true);
    cityWindow.ok->setText(str.string[50]);
    cityWindow.cancel->setText(str.string[51]);
    for(int i = 0; i < 9; i++){
        cityWindow.prodOpt[i]->setText(str.string[17 + i]);
        connect(cityWindow.prodOpt[i], SIGNAL(clicked()), this, SLOT(doUpdateProdLabel()));
    }
    connect(cityWindow.ok, SIGNAL(released()), this, SLOT(doSetCityProduction()));

    /* Rankings window */
    rankingWindow.setWindowTitle(str.string[95]);
    for(int i = 0; i < 6; i++){
        rankingWindow.item[(i + 1) * 4]->setIcon(QPixmap::fromImage(unitImage[i * 9]));
        if(i < 4) rankingWindow.item[i]->setText(str.string[i + 88]);
    }
    rankingWindow.ok->setText(str.string[50]);

    /* Goto Hex window */
    gotoWindow.setWindowTitle(str.string[122]);
    gotoWindow.label[0]->setText(str.string[123]);
    gotoWindow.label[1]->setText(str.string[124]);
    gotoWindow.ok->setText(str.string[50]);
    gotoWindow.cancel->setText(str.string[51]);
    connect(gotoWindow.ok, SIGNAL(released()), this, SLOT(doGotoHex()));

    /* Highscores window */
    highscoreWindow.setWindowTitle(str.string[96]);
    for(int i = 0; i < 7; i++)
        highscoreWindow.item[i]->setText(str.string[107+i]);
    highscoreWindow.ok->setText(str.string[50]);

    /* Load/save dialogs */
    fileDialog = new QFileDialog(this);
    fileDialog->setNameFilter(str.string[61]);

    /* Message boxes */
    ingameQuestion.setStandardButtons(QMessageBox::Yes);
    ingameQuestion.addButton(QMessageBox::No);
    ingameQuestion.setDefaultButton(QMessageBox::No);
    ingameQuestion.setText(str.string[16]);
    ingameQuestion.setIcon(QMessageBox::Question);
    ingameInfo = new QMessageBox;
}

/* Create menus */
void MainWind::createMenus(){
    for(int i = 0; i < 10; i++){
        if(i < 7) menuAction[i] = new QAction(str.string[54+i], this);
        else menuAction[i] = new QAction(str.string[88+i]+"...", this);
    }
    menuAction[10] = new QAction(str.string[80], this);
    menuAction[11] = new QAction(str.string[122], this);

    for(int i = 0; i < 3; i++){
        option[i] = new QAction(str.string[125 + i], this);
        option[i]->setCheckable(true);
    }
    connect(option[1], SIGNAL(changed()), this, SLOT(doUpdateBorderPen()));
    loadOptions();

    connect(menuAction[0], SIGNAL(triggered()), this, SLOT(doShowNewGameWindow()));
    connect(menuAction[1], SIGNAL(triggered()), this, SLOT(doLoadGame()));
    connect(menuAction[2], SIGNAL(triggered()), this, SLOT(doSaveGame()));
    connect(menuAction[3], SIGNAL(triggered()), this, SLOT(doClose()));
    connect(menuAction[4], SIGNAL(triggered()), this, SLOT(doShowGameInfo()));
    connect(menuAction[5], SIGNAL(triggered()), this, SLOT(doVisitWWW()));
    connect(menuAction[6], SIGNAL(triggered()), this, SLOT(doManual()));
    connect(menuAction[7], SIGNAL(triggered()), this, SLOT(doShowRankings()));
    connect(menuAction[8], SIGNAL(triggered()), this, SLOT(doShowHighScores()));
    connect(menuAction[9], SIGNAL(triggered()), this, SLOT(doShowLog()));
    connect(menuAction[10], SIGNAL(triggered()), this, SLOT(doSurrender()));
    connect(menuAction[11], SIGNAL(triggered()), this, SLOT(doShowGoto()));

    menuAction[0]->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    menuAction[1]->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton));
    menuAction[2]->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
    menuAction[3]->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    menuAction[4]->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
    menuAction[6]->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogHelpButton));

    for(int i = 0; i < 4; i++)
        menu[i] = menuBar()->addMenu("");
    menu[0]->setTitle(str.string[52]);
    menu[1]->setTitle(str.string[94]);
    menu[2]->setTitle(str.string[128]);
    menu[3]->setTitle(str.string[53]);

    menu[0]->addAction(menuAction[0]);
    menu[0]->addSeparator();
    menu[0]->addAction(menuAction[1]);
    menu[0]->addAction(menuAction[2]);
    menu[0]->addSeparator();
    menu[0]->addAction(menuAction[3]);
    menu[1]->addAction(menuAction[10]);
    menu[1]->addAction(menuAction[11]);
    menu[1]->addSeparator();
    menu[1]->addAction(menuAction[7]);
    menu[1]->addAction(menuAction[8]);
    menu[1]->addSeparator();
    menu[1]->addAction(menuAction[9]);
    for(int i = 0; i < 3; i++)
        menu[2]->addAction(option[i]);
    menu[3]->addAction(menuAction[4]);
    menu[3]->addSeparator();
    menu[3]->addAction(menuAction[5]);
    menu[3]->addAction(menuAction[6]);

    menuAction[2]->setDisabled(true);
    menuAction[10]->setDisabled(true);
    menuAction[11]->setDisabled(true);
    menuAction[7]->setDisabled(true);
}

/* Load program options from 'hexwars.dat' file */
void MainWind::loadOptions(){
    u_int8_t opt;
    QFile file(QCoreApplication::applicationDirPath() + "/hexwars.dat");
    LOG(INFO, str.log[24]);
    if(file.open(QIODevice::ReadOnly)){
        QDataStream in(&file);
        in >> opt;
        file.close();
        option[0]->setChecked(opt & 1);
        option[1]->setChecked(opt & 2);
        option[2]->setChecked(opt & 4);
    }
    /* If we can't load the file, set default config
     * and attempt to create the file */
    else{
        LOG(WARN, str.log[25]);
        option[0]->setChecked(true);
        option[1]->setChecked(true);
        option[2]->setChecked(false);
        if(file.open(QIODevice::WriteOnly)){
            QDataStream out(&file);
            opt = option[0]->isChecked() | (option[1]->isChecked() * 2)
                    | (option[2]->isChecked() * 4);
            out << (quint8)opt;
            file.close();
        }
    }
}

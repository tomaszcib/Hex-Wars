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
 *  /ui/dock.cpp
 *
 *  GUI procedures for left-side "dock" area.
 *  NOTE: game's "dock" is not an actual QDockArea, all the widgets have
 *  MainWind class as their parent and are simply moved into the left side
 *  of the window.
 *
 * *************************************************************************/

#include "../mainwind.h"

void MainWind::createDock(){

    /* Minimap */
    titleDock[0] = new QLabel(this);
    titleDock[0]->setFixedWidth(200);
    titleDock[0]->move(15, 15);
    minimapScene = new QGraphicsScene(this);
    minimapView = new QGraphicsView(this);
    minimapView->setScene(minimapScene);
    minimapView->move(15, 45);
    minimapView->setFixedSize(200, 200);
    minimapBackground = minimapScene->addPixmap(QPixmap::fromImage(minimap.image));
    minimapVertP = minimapScene->addLine(0, -10, 0, 10, QPen(Qt::white));
    minimapHoriP = minimapScene->addLine(-10, 0, 10, 0, QPen(Qt::white));
    minimapScene->setSceneRect(0, 0, 200, 200);
    minimapView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    minimapView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    /* Selected unit */
    titleDock[1] = new QLabel(this);
    titleDock[1]->setFixedWidth(200);
    titleDock[1]->move(15, 255);
    dockImage[1] = new QLabel(this);
    dockImage[1]->setFixedSize(30, 30);
    dockImage[1]->move(15, 290);
    for(int i = 0; i < 3; i++){
        unitData[i] = new QLabel(this);
        unitData[i]->setFixedWidth(130);
        unitData[i]->move(60, 280 + i * 15);
    }

    QPixmap iconSetPix;
    iconSetPix = QPixmap(":/images/icons.png", "PNG");
    for(int i = 0; i < 8; i++){
        panelAction[i] = new QPushButton(this);
        panelAction[i]->setIcon(iconSetPix.copy(i * 30 + i, 0, 30, 30));
        panelAction[i]->move(30 + ((i%4) * 40 + (i%4)), 345 + 40 * (i / 4) + i / 4);
        panelAction[i]->setFixedSize(40, 40);
        panelAction[i]->setIconSize(QSize(30, 30));
        panelAction[i]->setDisabled(true);
        panelAction[i]->setToolTip(str.string[6 + i]);
    }

    /* Chart */
    chart = QPixmap(200, 125);
    titleDock[2] = new QLabel(this);
    titleDock[2]->setFixedWidth(200);
    titleDock[2]->move(15, 440);
    dockImage[3] = new QLabel(this);
    dockImage[3]->setFixedSize(200, 125);
    dockImage[3]->move(15, 470);

    connect(panelAction[0], SIGNAL(released()), this, SLOT(doPrevUnit()));
    connect(panelAction[1], SIGNAL(released()), this, SLOT(doNextUnit()));
    connect(panelAction[2], SIGNAL(released()), this, SLOT(doDeselect()));
    connect(panelAction[3], SIGNAL(released()), this, SLOT(doLandParas()));
    connect(panelAction[4], SIGNAL(released()), this, SLOT(doCenterCameraAt()));
    connect(panelAction[5], SIGNAL(released()), this, SLOT(doDelete()));
    connect(panelAction[6], SIGNAL(released()), this, SLOT(doCityCloseUp()));
    connect(panelAction[7], SIGNAL(released()), this, SLOT(doEndTurn()));

    titleDock[0]->setText(str.string[0]);
    titleDock[1]->setText(str.string[1]);
    titleDock[2]->setText(str.string[5]);

}

void MainWind::drawDockChart(){
    QPainter painter(&chart);
    chart.fill(QColor(200, 200, 200));
    for(int i = 0; i < 6; i++){
        painter.setBrush(QColor(player[i].color));
        painter.drawRect(i * 33, 124, 33, -(player[i].citiesControl * 124 / city.size()));
    }
    dockImage[3]->setPixmap(chart);
}

void MainWind::updateSelection(){
    dockImage[1]->setPixmap(QPixmap::fromImage(unitImage[hexagon[curHex].unit[0].type + hexagon[curHex].unit[0].owner * 9]));
    titleDock[1]->setText(str.string[1]);
    unitData[0]->setText(str.string[2] + QString::number(hexagon[curHex].unit[0].HP) + "/ 100");
    unitData[1]->setText(str.string[3] + QString::number(hexagon[curHex].unit[0].moves));
    unitData[2]->setText(str.string[4] + ((hexagon[curHex].unit[0].type >= 1 && hexagon[curHex].unit[0].type <= 3)
            ? QString::number(hexagon[curHex].unit[0].fuel) : "-"));
}

void MainWind::updateNowPlaying(){
    dockImage[1]->setPixmap(QPixmap::fromImage(unitImage[63 + curPlayer]));
    titleDock[1]->setText(str.string[16]);
    if(lastHumanPlayer == 7) unitData[0]->setText("(" + str.string[71] + ")");
    else unitData[0]->setText("(" + str.string[player[curPlayer].level + 40] + ")");
    unitData[1]->setText("");
    unitData[2]->setText("");
}

void MainWind::updateMinimap(){
    minimapBackground->setPixmap(QPixmap::fromImage(minimap.image.scaled(200, 200)));
}

void MainWind::updateMinimapPointer(){
    minimapHoriP->setLine(cameraPos.x() * minimap.pixelScale - 10, cameraPos.y() * minimap.pixelScale,
                          cameraPos.x() * minimap.pixelScale + 10, cameraPos.y() * minimap.pixelScale);
    minimapVertP->setLine(cameraPos.x() * minimap.pixelScale, cameraPos.y() * minimap.pixelScale - 10,
                          cameraPos.x() * minimap.pixelScale, cameraPos.y() * minimap.pixelScale + 10);

}

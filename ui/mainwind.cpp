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
 *  /ui/mainwind.cpp
 *
 *  game's main window routines - MainWind constructor, mouse and paint events.
 *
 * *************************************************************************/

#include "../mainwind.h"

MainWind::MainWind(){

    /* Test sound */
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_VolumeMusic(32);

    /* Game version */
    GAME_VERSION = 1;
    LOG(INFO, str.log[0]);

    R.srand(QTime(0,0,0).secsTo(QTime::currentTime()));
    setMouseTracking(true);
    setBackgroundRole(QPalette::Base);
    itemInMotion = 0;

    player[0].color = PLAYER_RED;
    player[1].color = PLAYER_GREEN;
    player[2].color = PLAYER_BLUE;
    player[3].color = PLAYER_YELLOW;
    player[4].color = PLAYER_CYAN;
    player[5].color = PLAYER_MAGENTA;

    setWindowTitle(tr("Hex Wars"));
    setWindowIcon(QIcon(":images/HexWarsIcon.png"));
    resize(800, 600);
    createHexagonPath();
    createTextures();
    createDock();
    createWindows();
    createMenus();
    highscore.resize(10);
    highScoreLoad();
    hexagon = new Hexagon[MAX_MAPSIZE * MAX_MAPSIZE];
    allyUnit = new u_int16_t[MAX_MAPSIZE * MAX_MAPSIZE];
    enemyUnit = new u_int16_t[MAX_MAPSIZE * MAX_MAPSIZE];
    transList = new u_int16_t*[MAX_MAPSIZE * MAX_MAPSIZE];
    carrierList = new u_int16_t*[MAX_MAPSIZE * MAX_MAPSIZE];
    curHex = -1;
    SIZEOF_MAP = 0;
    transportReqId = 0;
    cameraPos = QPoint((this->width() + 231) / 2, this->height());
    doUpdateBorderPen();
}


void MainWind::createHexItem(const QPainterPath &path, const QPoint &pos) {
    HexItem shapeItem;
    shapeItem.setPath(path);
    shapeItem.setPosition(pos);
    shapeItems.append(shapeItem);
}

QPoint MainWind::initialItemPosition(const QPainterPath &path){
    int x;
    int y = (height() - (int)path.controlPointRect().height()) / 2;
    if (shapeItems.size() == 0)
        x = ((3 * width()) / 2 - (int)path.controlPointRect().width()) / 2;
    else
        x = (width() / shapeItems.size() - (int)path.controlPointRect().width()) / 2;

    return QPoint(x, y);
}


bool MainWind::event(QEvent *event){
    return QWidget::event(event);
}


void MainWind::paintEvent(QPaintEvent * /* event */){
    QPainter painter(this);
    QBrush brush;
    painter.setPen(borderPen);
    painter.setClipRect(231, 0, this->width() - 231, this->height());
    for(int i = 0; i < SIZEOF_MAP; i++){
        brush.setTextureImage(hexagon[i].texture);
        painter.setBrush(brush);
        painter.translate(shapeItems[i].position());
        painter.drawPath(hexagonPath);
        painter.translate(-shapeItems[i].position());
    }
}


void MainWind::mousePressEvent(QMouseEvent *event){
    int index;
    if (event->button() == Qt::LeftButton) {
        index = itemAt(event->pos());
        selectNothingEffect();
        if(hexExists(index) && curPlayer == lastHumanPlayer){
            if(!hexagon[index].unit.empty() && isContainingAlly(index, curPlayer)){
                curHex = index;
                selectUnitEffect();
                paintHex(curHex);
                update();
            }
            else if(hexagon[index].unit.empty() && isContainingAlly(index, curPlayer)){
                curCity = hexagon[index].cityId;
                cityWindow.prodOpt[city[curCity].produces]->setChecked(true);
                cityWindow.setWindowIcon(QPixmap::fromImage(unitImage[54 + lastHumanPlayer]));
                doUpdateProdLabel();
                cityWindow.show();
            }
        }
    }


    else if(event->button() == Qt::RightButton){
        previousPosition = event->pos();
    }
    update();
}


void MainWind::mouseMoveEvent(QMouseEvent *event){
    if ((event->buttons() & Qt::RightButton)){
        QPoint offset;
        offset = event->pos() - previousPosition;
        if(abs(offset.x()) > 21 || abs(offset.y()) > 21){
            dragMode = true;
            setCursor(Qt::ClosedHandCursor);
            offset *= 1.5;
            for(int i=0; i<SIZEOF_MAP; i++){
                itemInMotion = &shapeItems[i];
                itemInMotion->setPosition(itemInMotion->position() + offset);
            }
            cameraPos = cameraPos - (offset);
            updateMinimapPointer();
            previousPosition = event->pos();
            update();
        }
    }
}


void MainWind::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        if(dragMode){
            dragMode = false;
            setCursor(Qt::ArrowCursor);
        }
        else{
            if(curHex == -1) return;
            int index = itemAt(event->pos());
            if(hexExists(index)){
                if(!hexagon[curHex].unit.empty()){
                    destHex = index;
                    destHex = moveUnit(curHex, destHex);
                    selectNothingEffect();
                    curHex = destHex;
                    paintHex(curHex);
                    selectUnitEffect();
                    update();
                }
            }
        }
    }
}

int MainWind::itemAt(const QPoint &pos){
    for (int i = shapeItems.size() - 1; i >= 0; --i) {
        const HexItem &item = shapeItems[i];
        if (item.path().contains(pos - item.position()))
            return i;
    }
    return -1;
}

void MainWind::moveItemTo(const QPoint &pos){
    QPoint offset = pos - previousPosition;
    itemInMotion->setPosition(itemInMotion->position() + offset);
    previousPosition = pos;
    update();
}

void MainWind::selectNothingEffect(){
    int prevHex;
    if(curHex != -1){
        if(!hexagon[curHex].unit.empty()){
        /* If we deselect unit carried on a ship, the ship is put on the top of the stack */
            if(hexagon[curHex].unit[0].type == TANK && hasFriendlyShip(curHex, curPlayer, TRANSPORT)
                    && hexagon[curHex].height <= WATER_LEVEL)
                putShipOnTop(curHex, TRANSPORT);
            else if(hexagon[curHex].unit[0].type == FIGHTER
                    && hasFriendlyShip(curHex, curPlayer, CARRIER) && hexagon[curHex].height <= WATER_LEVEL)
                putShipOnTop(curHex, CARRIER);
        }
        prevHex = curHex;
        curHex = -1;
        for(int i = 0; i < 7; i++)
            panelAction[i]->setDisabled(true);
        updateNowPlaying();
        paintHex(prevHex);
        update();
    }
}

void MainWind::selectUnitEffect(){
    if(hexagon[curHex].unit.size() > 1){
        panelAction[0]->setDisabled(false);
        panelAction[1]->setDisabled(false);
    }
    panelAction[2]->setDisabled(false);
    if(hexagon[curHex].unit[0].type == 1 && canLandParas(curHex))
        panelAction[3]->setDisabled(false);
    panelAction[4]->setDisabled(false);
    panelAction[5]->setDisabled(false);
    if(hexagon[curHex].terrain == 1){
        panelAction[6]->setDisabled(false);
        curCity = hexagon[curHex].cityId;
    }
    updateSelection();
}


/* Move the camera towards the hex */
void MainWind::moveCameraTo(int hex){
    QPoint offset, pos;
    pos = previousPosition - shapeItems[hex].position() + QPoint(this->width()/2, this->height()/2);
    for(int i = 0; i < SIZEOF_MAP; i++){
        itemInMotion = &shapeItems[i];
        offset = pos - previousPosition;
        itemInMotion->setPosition(itemInMotion->position() + offset);
    }
    cameraPos = hexPosition(hex) + QPoint(0, this->height() / 2);
    updateMinimapPointer();
    previousPosition = pos;
    update();
}

/* Create MainWindow caption */
void MainWind::createMainWindTitle(){
    setWindowTitle("Hex Wars (world " + QString::number(gseed) + ", turn " + QString::number(curTurn) +")");
}

/* Exit game - ask a user if he/she wants to quit */
void MainWind::closeEvent(QCloseEvent *event){
    QFile file("./hexwars.dat");
    ingameQuestion.setWindowTitle(str.string[119]);
    ingameQuestion.setText(str.string[120]);
    if(ingameQuestion.exec() == QMessageBox::Yes){
        /* Attempt to save the program settings */
        if(file.open(QIODevice::WriteOnly)){
            u_int8_t opt;
            QDataStream out(&file);
            opt = (u_int8_t)option[0]->isChecked()
                  | ((u_int8_t)option[1]->isChecked() * 2)
                  | ((u_int8_t)option[2]->isChecked() * 4);
            out << (quint8)opt;
            file.close();
        }
        /* Exit the program */
        LOG(INFO, str.log[23]);
        event->accept();
    }
    /* Cancel exitting */
    else event->ignore();
}

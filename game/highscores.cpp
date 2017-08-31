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
 *  /game/highscores.cpp
 *
 *  loading and saving highscores table.
 *
 * *************************************************************************/

#include "mainwind.h"

/* Load high scores from file */
void MainWind::highScoreLoad(){
    bool attempt = false;
    quint8 size;
    quint8 t8[4];   //8-bit buffer
    quint32 t32[3]; //32-bit buffer
    QFile file(QCoreApplication::applicationDirPath() + "/scores.dat");
    clearScores();
    LOG(INFO, str.log[1]);
    check:
    if(!file.open(QIODevice::ReadOnly)){
        if(!file.open(QIODevice::WriteOnly) && attempt){
            LOG(ERROR, str.string[106]);
            ingameInfo->setWindowTitle(str.string[73]);
            ingameInfo->setText(str.string[106] + "'" + qApp->applicationDirPath() + "'.");
            ingameInfo->setIcon(QMessageBox::Warning);
            ingameInfo->exec();
            return;
        }
        LOG(WARN, str.log[2]);
        QDataStream out(&file);
        out << (quint8)0;
        file.close();
        attempt = true;
        goto check;
    }
    QDataStream in(&file);
    in >> t8[0];
    size = t8[0];
    for(int i = 0; i < size; i++){
        in >> t8[0] >> t8[1] >> t8[2] >> t8[3]
           >> t32[0] >> t32[1] >> t32[2];
        highscore[i].size = t8[0];
        highscore[i].land = t8[1];
        highscore[i].color = t8[2];
        highscore[i].diff = t8[3];
        highscore[i].world = t32[0];
        highscore[i].turns = t32[1];
        highscore[i].score = t32[2];
        in >> t8[0];
        for(int j = 0; j < t8[0]; j++){
            in >> t8[1];
            highscore[i].name.append((char)t8[1]);
        }
    }
    updateHScoresWindow();
    file.close();
}

/* Check if player 'a' qualify to be placed in the high score table */
void MainWind::highScoreSave(u_int8_t a){
    QFile file(QCoreApplication::applicationDirPath() + "/scores.dat");
    bool ok;
    HighScoreItem newscore;
    unsigned int score = player[a].scoreKilled / 3 + player[a].scoreProduced / 10;
    for(int i = 0; i < 10; i++){
        if(score > highscore[i].score){
            newscore.size = rows;
            newscore.land = tAmount;
            newscore.color = a;
            newscore.diff = gdiff;
            newscore.world = gseed;
            newscore.turns = curTurn;
            newscore.score = score;
            newscore.name = QInputDialog::getText(this, str.string[104], str.string[105], QLineEdit::Normal,
                                                  QDir::home().dirName(), &ok);
            if(ok && !newscore.name.isEmpty()){
                highscore.insert(i, newscore);
                if(highscore.size() > 10)
                    highscore.pop_back();
                if(!file.open(QIODevice::WriteOnly)){
                    LOG(ERROR, str.string[106]);
                    ingameInfo->setWindowTitle(str.string[73]);
                    ingameInfo->setText(str.string[106] + "'" + qApp->applicationDirPath() + "'.");
                    ingameInfo->setIcon(QMessageBox::Warning);
                    ingameInfo->exec();
                    return;
                }
                QDataStream out(&file);
                out << (quint8)highscore.size();
                for(int i = 0; i < 10; i++){
                    out << (quint8)highscore[i].size
                        << (quint8)highscore[i].land
                        << (quint8)highscore[i].color
                        << (quint8)highscore[i].diff
                        << (quint32)highscore[i].world
                        << (quint32)highscore[i].turns
                        << (quint32)highscore[i].score
                        << (quint8)highscore[i].name.length();
                    for(int j = 0; j < highscore[i].name.length(); j++)
                        out << (quint8)highscore[i].name.at(j).toLatin1();
                }
                updateHScoresWindow();
                return;
            }
        }
    }
}

/* Clear scores */
void MainWind::clearScores(){
    for(int i = 0; i < 10; i++){
        highscore[i].name = "";
        highscore[i].size = 0;
        highscore[i].land = 0;
        highscore[i].color = 0;
        highscore[i].diff = 0;
        highscore[i].world = 0;
        highscore[i].turns = 0;
        highscore[i].score = 0;
    }
}

/* Update scores window */
void MainWind::updateHScoresWindow(){
    HighScoreItem *h;
    int8_t tmp, i, j, k;
    for(i = 0; i < 10; i++){
        h = &highscore[i];
        k = i + 1;
        if(h->name.isEmpty()){
            for(j = 0; j < 7; j++)
                highscoreWindow.item[k*7+j]->setText("-");
        }
        else{
            highscoreWindow.item[k*7]->setText(h->name);
            highscoreWindow.item[k*7]->setIcon(QPixmap::fromImage(unitImage[h->color * 9]));
            highscoreWindow.item[k*7+1]->setText(QString::number(h->world));
            highscoreWindow.item[k*7+2]->setText(QString::number(h->size)+"x"+QString::number(h->size));
            if(h->size > h->land) tmp = 2;
            else if(h->size == h->land) tmp = 1;
            else tmp = 0;
            highscoreWindow.item[k*7+3]->setText(str.string[tmp + 35]);
            highscoreWindow.item[k*7+4]->setText(h->diff == 255 ? str.string[99] : QString::number(h->diff)+"%");
            highscoreWindow.item[k*7+5]->setText(QString::number(h->turns));
            highscoreWindow.item[k*7+6]->setText(QString::number(h->score));
        }
    }
}

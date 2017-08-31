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
 *  /ui/dialog.h
 *
 *  definitions of in-game dialog windows classes. See /game/dialog.cpp
 *  for more details.
 *
 * *************************************************************************/

#ifndef DIALOG_H
#define DIALOG_H
//#include <QDialog>
//#include <QtGui>
#include <QtWidgets>

class CityCloseUp:public QDialog{
    Q_OBJECT
public:
    CityCloseUp(QWidget *parent = 0);
    QBoxLayout *mainLayout, *buttonLayout, *optionsLayout, *prodSubLayout;
    QPushButton *ok, *cancel;
    QLabel *curProd, *curProdIcon;
    QGroupBox *prodSelect;
    QRadioButton *prodOpt[9];
};

class NewGame:public QDialog{
    Q_OBJECT
public:
    NewGame(QWidget *parent = 0);
    QBoxLayout *mainLayout, *generatorLayout, *generatorColumn[2], *playerLayout, *playerColumn[2], *buttonLayout;
    QLabel *generatorLabel[4], *playerLabel[6], *difficulty;
    QSpinBox *seed;
    QComboBox *genOpt[3], *playerOpt[6];
    QPushButton *ok, *cancel, *editRules;
    QGroupBox *genGroup, *playerGroup;
    QIntValidator *seedValidator;

};

class EditRules:public QDialog{
    Q_OBJECT
public:
    EditRules(QWidget *parent = 0);
    QBoxLayout *row[10], *rowLayout, *buttonLayout, *mainLayout;
    QLabel *unitName[9], *title[4];
    QLineEdit *prod[9], *moves[9], *fuel[9];
    QPushButton *ok, *cancel, *defaults;
    QIntValidator *valueValidator, *fuelValidator;
};

class Rankings:public QDialog{
    Q_OBJECT
public:
    Rankings(QWidget *parent = 0);
    QBoxLayout *mainLayout, *buttonLayout;
    QTableWidget *ranking;
    QTableWidgetItem *item[28];
    QPushButton *ok;

};

class Highscores:public QDialog{
    Q_OBJECT
public:
    Highscores(QWidget *parent = 0);
    QBoxLayout *mainLayout, *buttonLayout;
    QTableWidget *ranking;
    QTableWidgetItem *item[77];
    QPushButton *ok;
};

class Programlog:public QDialog{
    Q_OBJECT
public:
    Programlog(QWidget *parent = 0);
    void append(QString &str);
    QBoxLayout *mainLayout, *buttonLayout;
    QTextDocument *doc;
    QString output;
    QTextEdit *outfield;
    QPushButton *ok;
};

class GotoHex:public QDialog{
    Q_OBJECT
public:
    GotoHex(QWidget *parent = 0);
    QBoxLayout *mainLayout, *rowLayout[2], *buttonLayout;
    QSpinBox *spin[2];
    QLabel *label[2];
    QPushButton *ok, *cancel;
};

#endif // DIALOG_H

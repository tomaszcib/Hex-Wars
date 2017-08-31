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
 *  /ui/dialog.cpp
 *
 *  constructors for included dialog windows classes, which are declared in /ui/dialog.h
 *  header file.
 *
 * *************************************************************************/

#include "dialog.h"

CityCloseUp::CityCloseUp(QWidget *parent):QDialog(parent){
    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    optionsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    prodSubLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);

    prodSelect = new QGroupBox;
    curProdIcon = new QLabel;
    curProd = new QLabel;
    prodSubLayout->addSpacing(50);
    prodSubLayout->addWidget(curProdIcon);
    prodSubLayout->addWidget(curProd);
    prodSubLayout->addSpacing(40);

    optionsLayout->addLayout(prodSubLayout);

    for(int i = 0; i < 9; i++){
        prodOpt[i] = new QRadioButton;
        optionsLayout->addWidget(prodOpt[i]);
    }
    prodSelect->setLayout(optionsLayout);

    ok = new QPushButton("OK");
    cancel = new QPushButton("Cancel");
    buttonLayout->addWidget(ok);
    buttonLayout->addWidget(cancel);
    connect(cancel, SIGNAL(released()), this, SLOT(close()));

    mainLayout->addWidget(prodSelect);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setFixedSize(250,370);
    setModal(true);
}

NewGame::NewGame(QWidget *parent):QDialog(parent){
    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    generatorLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    playerLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    playerLayout->addSpacing(50);
    for(int i = 0; i < 2; i++){
        generatorColumn[i] = new QBoxLayout(QBoxLayout::TopToBottom);
        generatorLayout->addLayout(generatorColumn[i]);
        playerColumn[i] = new QBoxLayout(QBoxLayout::TopToBottom);
        playerLayout->addLayout(playerColumn[i]);
    }
    for(int i = 0; i < 4; i++){
        generatorLabel[i] = new QLabel;
        generatorColumn[0]->addWidget(generatorLabel[i]);
        if(!i) {
            seed = new QSpinBox;
            seed->setRange(0, 16777216);
            generatorColumn[1]->addWidget(seed);
        }
        else{
            genOpt[i-1] = new QComboBox;
            generatorColumn[1]->addWidget(genOpt[i-1]);
        }
    }
    for(int i = 0; i < 6; i++){
        playerLabel[i] = new QLabel;
        playerColumn[0]->addWidget(playerLabel[i]);
        playerOpt[i] = new QComboBox;
        playerColumn[1]->addWidget(playerOpt[i]);
    }
    difficulty = new QLabel;
    ok = new QPushButton;
    cancel = new QPushButton;
    connect(cancel, SIGNAL(released()), this, SLOT(close()));
    editRules = new QPushButton;
    buttonLayout->addWidget(ok);
    buttonLayout->addWidget(cancel);
    buttonLayout->addWidget(editRules);

    genGroup = new QGroupBox;
    genGroup->setLayout(generatorLayout);
    playerGroup = new QGroupBox;
    playerGroup->setLayout(playerLayout);

    mainLayout->addWidget(genGroup);
    mainLayout->addWidget(playerGroup);
    mainLayout->addWidget(difficulty);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setFixedSize(300, 400);
    setModal(true);
}

EditRules::EditRules(QWidget *parent):QDialog(parent){
    rowLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    for(int i = 0; i < 10; i++){
        row[i] = new QBoxLayout(QBoxLayout::LeftToRight);
        rowLayout->addLayout(row[i]);
    }

    valueValidator = new QIntValidator(1, 32);
    fuelValidator = new QIntValidator(1, 4);

    for(int i = 0; i < 4; i++){
        title[i] = new QLabel;
        row[0]->addWidget(title[i]);
    }
    title[0]->setFixedWidth(100);
    for(int i = 0; i < 9; i++){
        unitName[i] = new QLabel;
        unitName[i]->setFixedWidth(100);
        prod[i] = new QLineEdit;
        moves[i] = new QLineEdit;
        fuel[i] = new QLineEdit;
        if(i < 1 || i > 3) fuel[i]->setDisabled(true);
        prod[i]->setValidator(valueValidator);
        moves[i]->setValidator(valueValidator);
        fuel[i]->setValidator(fuelValidator);
        row[i+1]->addWidget(unitName[i]);
        row[i+1]->addWidget(prod[i]);
        row[i+1]->addWidget(moves[i]);
        row[i+1]->addWidget(fuel[i]);
    }

    ok = new QPushButton;
    cancel = new QPushButton;
    connect(cancel, SIGNAL(released()), this, SLOT(close()));
    defaults = new QPushButton;
    buttonLayout->addWidget(ok);
    buttonLayout->addWidget(cancel);
    buttonLayout->addWidget(defaults);
    mainLayout->addLayout(rowLayout);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setFixedSize(300, 370);
    setModal(true);

}

Rankings::Rankings(QWidget *parent):QDialog(parent){
    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    ranking = new QTableWidget(6, 4);
    for(int i = 0; i < 28; i++){
        item[i] = new QTableWidgetItem(QString::number(i));
        item[i]->setFlags(Qt::ItemIsEnabled);
        if(i < 4) ranking->setHorizontalHeaderItem(i, item[i]);
        else ranking->setItem(i/4 - 1, i%4, item[i]);
    }
    ranking->setColumnWidth(0, 120);
    ranking->setColumnWidth(1, 70);
    ranking->setColumnWidth(2, 70);
    ranking->horizontalHeader()->setStretchLastSection(true);
    //ranking->setColumnWidth(3, 80);
    //ranking->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    //ranking->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ranking->verticalHeader()->setVisible(false);
    ok = new QPushButton("OK");
    ok->setFixedWidth(60);
    connect(ok, SIGNAL(released()), this, SLOT(close()));
    buttonLayout->addWidget(ok);
    mainLayout->addWidget(ranking);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setFixedSize(400, 270);
    setModal(true);
}

Highscores::Highscores(QWidget *parent):QDialog(parent){
    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    ranking = new QTableWidget(10, 7);
    for(int i = 0; i < 77; i++){
        item[i] = new QTableWidgetItem(QString::number(i));
        item[i]->setFlags(Qt::ItemIsEnabled);
        if(i < 7) ranking->setHorizontalHeaderItem(i, item[i]);
        else ranking->setItem(i/7 - 1, i%7, item[i]);
    }
    ranking->setColumnWidth(0, 120);
    ranking->setColumnWidth(1, 100);
    ranking->setColumnWidth(2, 60);
    ranking->setColumnWidth(3, 60);
    ranking->setColumnWidth(4, 60);
    ranking->setColumnWidth(5, 80);
    ranking->horizontalHeader()->setStretchLastSection(true);
    ok = new QPushButton("OK");
    ok->setFixedWidth(60);
    connect(ok, SIGNAL(released()), this, SLOT(close()));
    buttonLayout->addWidget(ok);
    mainLayout->addWidget(ranking);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    setFixedSize(600, 380);
    setModal(true);
}

Programlog::Programlog(QWidget *parent):QDialog(parent){
    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    doc = new QTextDocument;
    outfield = new QTextEdit;
    outfield->setReadOnly(true);
    outfield->setAcceptRichText(false);
    ok = new QPushButton("OK");
    ok->setFixedWidth(60);
    connect(ok, SIGNAL(released()), this, SLOT(close()));
    buttonLayout->addWidget(ok);
    mainLayout->addWidget(outfield);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    setFixedSize(550, 400);
    setModal(true);
}

/* Add new log string to programlog window */
void Programlog::append(QString &str){
    output.append(str);
    if(output.size() > 1024)
        output.remove(0, output.size() - 1024);
    doc->setPlainText(output);
    outfield->setDocument(doc);
}

GotoHex::GotoHex(QWidget *parent):QDialog(parent){
    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    for(int i = 0; i < 2; i++){
        rowLayout[i] = new QBoxLayout(QBoxLayout::LeftToRight);
        spin[i] = new QSpinBox;
        label[i] = new QLabel;
        rowLayout[i]->addWidget(label[i]);
        rowLayout[i]->addWidget(spin[i]);
        mainLayout->addLayout(rowLayout[i]);
    }
    ok = new QPushButton("OK");
    cancel = new QPushButton;
    connect(cancel, SIGNAL(released()), this, SLOT(close()));
    buttonLayout->addWidget(ok);
    buttonLayout->addWidget(cancel);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    setFixedSize(250, 150);
    setModal(true);
}

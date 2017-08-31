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
 *  /ui/hexitem.cpp
 *
 *  file related to /ui/hexitem.h header file. Definitions of HexItem class
 *  members. HexItem is a class used to store a single item for QPainter
 *  (contains QPainterPath and x,y coords).
 *  NOTE: this is a copy of ShapeItem class from Qt's example called 'tooltips'.
 *
 * *************************************************************************/


#include <QtWidgets>
//#include <QtGui>
#include "hexitem.h"

QPainterPath HexItem::path() const{
    return myPath;
}

QPoint HexItem::position() const{
    return myPosition;
}

QColor HexItem::color() const{
    return myColor;
}

QString HexItem::toolTip() const{
    return myToolTip;
}

void HexItem::setPath(const QPainterPath &path){
    myPath = path;
}

void HexItem::setToolTip(const QString &toolTip){
    myToolTip = toolTip;
}

void HexItem::setPosition(const QPoint &position){
    myPosition = position;
}

void HexItem::setColor(const QColor &color){
    myColor = color;
}


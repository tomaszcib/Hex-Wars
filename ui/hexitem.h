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
 *  /ui/hexitem.h
 *
 *  HexItem class definition. See /ui/hexitem.cpp for more details.
 *
 * *************************************************************************/

#ifndef HEXITEM_H
#define HEXITEM_H

#include <QColor>
#include <QPainterPath>
#include <QPoint>

//! [0]
class HexItem{
public:
    void setPath(const QPainterPath &path);
    void setToolTip(const QString &toolTip);
    void setPosition(const QPoint &position);
    void setColor(const QColor &color);

    QPainterPath path() const;
    QPoint position() const;
    QColor color() const;
    QString toolTip() const;

private:
    QPainterPath myPath;
    QPoint myPosition;
    QColor myColor;
    QString myToolTip;
};

#endif

/***************************************************************************
 *   Copyright (C) 2008 by P. Sereno                                       *
 *   http://www.sereno-online.com                                          *
 *   http://www.qt4lab.org                                                 *
 *   http://www.qphoton.org                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef QLED_H
#define QLED_H

#include <Qt>
#include <QWidget>
#include <QtDesigner/QDesignerExportWidget>


class QColor;

class QDESIGNER_WIDGET_EXPORT QLed : public QWidget
{
 Q_OBJECT
	Q_PROPERTY(bool value READ value WRITE setValue);
	Q_PROPERTY(QColor color READ color WRITE setColor);

public: 
    QLed(QWidget *parent = 0);
    bool value() const { return m_value; }
    QColor color() const { return m_color; }
    
public slots:
	void setValue(bool);
	void setColor(QColor);
	void toggleValue();

protected:
    bool m_value;
    QColor m_color;
    void paintEvent(QPaintEvent *event);

};

#endif

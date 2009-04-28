/*
**	THE ARMADEUS PROJECT
**
**  Copyright (2006)  The source forge armadeus project team
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
**	 ApfServer.cpp
**
**	author: carbure@users.sourceforge.net
*/

#include <iostream>

#ifndef NOSERVERGUI
#include <QApplication>
#include <QTextEdit>
#include <QDateTime>
#include <QTextDocument>
#include <QTextBlock>
#include <QPushButton>
#endif



#include "ApfServer.h"
#include "ApfServerListener.h"

//#include "ApfApplication.h"
#ifndef NOSERVERGUI

using namespace Ui;
#endif

ApfServer *ApfServer::gApfServer = 0;

//******************************************************************************

#ifndef NOSERVERGUI
ApfServer::ApfServer(): QDialog(), mApfServerListener()
#else
ApfServer::ApfServer() : mApfServerListener()
#endif
{
#ifndef NOSERVERGUI
    setupUi(this);
    mRedLed = new QLed(tab_2);
    mRedLed->setColor(QColor(200,0,0));
    mGreenLed = new QLed(tab_2);
    mGreenLed->setColor(QColor(0,200,0));
    mSlide = new QSlide(tab_2);
    mSlide->startTmr(true);
    mManometer = new ManoMeter(tab_2);
    mThermometer = new ThermoMeter(tab_2);
    mWidgetsLayout = new QGridLayout(tab_2);
    mWidgetsLayout->addWidget(mRedLed, 0, 0);
    mWidgetsLayout->addWidget(mGreenLed, 2, 0);
    QSpacerItem* spacerItem = new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mWidgetsLayout->addItem(spacerItem, 1, 0);
    mWidgetsLayout->addWidget(mManometer, 1, 1, 2, 1);
    mWidgetsLayout->addWidget(mSlide, 0, 1);
    mWidgetsLayout->addWidget(mThermometer, 0, 2, 3, 1);
#endif
    mApfServerListener = new ApfServerListener(0);
    PutServerDebugMessage(QString("Starting Apf server"));
    connect( mApfServerListener, SIGNAL( logText(const QString &) ), SLOT(PutServerDebugMessage(const QString &)) );
#ifndef NOSERVERGUI
    connect( QuitButton, SIGNAL(clicked()), this, SLOT(EndServer()) );
#endif
    gApfServer = this;
}

//******************************************************************************

ApfServer *ApfServer::getApfServerInstance()
{
    return gApfServer;
}

//******************************************************************************

void ApfServer::EndServer()
{
#ifndef NOSERVERGUI
    hide();
    qApp->exit(0);
#else
    exit(0);
#endif
}

//******************************************************************************

ApfServer::~ApfServer()
{
    if (0 != mApfServerListener)
    {
        delete mApfServerListener;
    }
}

//******************************************************************************

void ApfServer::PutServerDebugMessage(const char * str)
{
    PutServerDebugMessage (QString(str));
}

//******************************************************************************

void ApfServer::PutServerDebugMessage(const QString & str)
{
#ifndef NOSERVERGUI
    if (mTextEdit)
    {
        mTextEdit->append(str);
    }
#else
    std::cout << str.toAscii().data() << std::endl;
#endif
}

//******************************************************************************

void ApfServer::setLedValue(int id, bool state)
{
#ifndef NOSERVERGUI
    if (id == 0)
    {
        mGreenLed->setValue(state);
    }
    else
    {
        mRedLed->setValue(state);
    }
#else
    std::cout << "Setting " << (id ? "Red" : "Green") << " LED to " << state << std::endl;
#endif
}

//******************************************************************************

void ApfServer::setManometerValue(int value)
{
#ifndef NOSERVERGUI
    mManometer->setValue(value);
#else
    std::cout << "Setting Manometer to " << value << std::endl;
#endif
}

//******************************************************************************

void ApfServer::setThermometerValue(int value)
{
#ifndef NOSERVERGUI
    mThermometer->setValue(value);
#else
    std::cout << "Setting Thermometer to " << value << std::endl;
#endif
}

//******************************************************************************

void ApfServer::setSlideText(const QString & atext)
{
#ifndef NOSERVERGUI
    mSlide->setText(atext);
#else
    std::cout << "Setting LCD text to " << atext.toAscii().data() << std::endl;
#endif
}

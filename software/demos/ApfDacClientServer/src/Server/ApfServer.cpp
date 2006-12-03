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

ApfServer::ApfServer(): QDialog(),mApfServerListener()
#else
ApfServer::ApfServer() : mApfServerListener()
#endif

{

#ifndef NOSERVERGUI
	setupUi(this);
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
     if ( 0 != mApfServerListener)
     {
        delete mApfServerListener;
     }
}

//******************************************************************************

void ApfServer::PutServerDebugMessage(const char * str)
{
    PutServerDebugMessage (QString(str)) ;
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

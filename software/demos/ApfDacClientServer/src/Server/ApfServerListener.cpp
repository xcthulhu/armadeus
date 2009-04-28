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
**	 ApfServerListener.cpp
**
**	author: carbure@users.sourceforge.net
*/ 


#include <unistd.h>
#include <stdlib.h>

#include <QString>

#include "ApfServerListener.h"
#include "ApfServer.h"


//******************************************************************************

ApfServerListener::ApfServerListener(QObject *parent):
QTcpServer( parent ),mApfCore(0),mParent(parent)
{
    connect( this, SIGNAL( newConnection() ),this, SLOT( newIncommingConnection() ) );

    bool ok = listen(QHostAddress::Any, APF_SOCKET);
//	bool ok =setSocketDescriptor (APF_SOCKET);
    if ( !ok )
    {
        qWarning("Failed to bind to port %d", APF_SOCKET);
        exit(1);
    }
}

//******************************************************************************

ApfServerListener::~ApfServerListener()
{
}

//******************************************************************************

void ApfServerListener::newIncommingConnection()
{
    emit logText(QString("").sprintf("Incoming connexion on port %d",APF_SOCKET));
    if ( 0 == mApfCore )
    {
        QTcpSocket *clientConnection = this->nextPendingConnection();

        emit logText(QString("").sprintf("Starting the APF core"));
        mApfCore = new ApfCore (clientConnection, this);

        connect( mApfCore, SIGNAL( logText(const QString &) ), ApfServer::getApfServerInstance(),
                    SLOT(PutServerDebugMessage(const QString &)) );
        connect( mApfCore, SIGNAL( ApfCoreFinished() ), SLOT( coreDestroyed() ) );
        connect( mApfCore, SIGNAL(setLedValue(int, bool)), ApfServer::getApfServerInstance(), SLOT(setLedValue(int, bool)) );
        connect( mApfCore, SIGNAL(setManometerValue(int)), ApfServer::getApfServerInstance(), SLOT(setManometerValue(int)) );
        connect( mApfCore, SIGNAL(setThermometerValue(int)), ApfServer::getApfServerInstance(), SLOT(setThermometerValue(int)) );
        connect( mApfCore, SIGNAL(updateText(const QString &)), ApfServer::getApfServerInstance(), SLOT(setSlideText(const QString &)) );
    }
    else
    {
        emit logText(QString("").sprintf("APF core already started. closing socket"));
        close();
    }
}

//******************************************************************************

void ApfServerListener::coreDestroyed()
{
    if ( 0 != mApfCore )
    {
        emit logText(QString("").sprintf("APF core stopped\n"));
        delete mApfCore;
        mApfCore = 0;
    }
    usleep(2000000);
}

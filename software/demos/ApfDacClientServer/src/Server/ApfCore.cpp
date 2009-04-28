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
**	 ApfCore.cpp: Core for Apf server
**
**	author: carbure@users.sourceforge.net
*/

#include <stdio.h>
#include <unistd.h>

#include "ApfCore.h"
//#include "ApfApplication.h"
#include "ApfNetworkOperation.h"

#define tr(X) X // no translation ....

//******************************************************************************
 
ApfCore::ApfCore(QTcpSocket *clientConnection, QObject *parent ,const char *):
QTcpSocket( parent ), mApfCurrentOperation(0), mVersionTransfered(false),
mApfDacController( MIN_DAC, MAX_DAC, MIN_DAC, MAX_DAC)

{
    mSocket =clientConnection;
    mApfNetworkProtocol = new ApfNetworkProtocol(mSocket);
    connect( mSocket, SIGNAL(disconnected()), SLOT(deleteLater()) );
    connect( mSocket, SIGNAL( readyRead()),this,  SLOT(data()) );
    connect( mApfNetworkProtocol, SIGNAL( finished()),this,  SLOT(exec()) );
    connect( mApfNetworkProtocol, SIGNAL( unexpectedCode()),this,  SLOT(protocolError()) );
    QTimer::singleShot ( 10000, this, SLOT(protocolError()) );
    mApfDacController.setbothDAC(MIN_DAC);
}

//******************************************************************************

ApfCore::~ApfCore()
{
    mApfDacController.setbothDAC(MIN_DAC);
    if ( 0 != mApfNetworkProtocol )
    {
        delete mApfNetworkProtocol;
    }
}

//******************************************************************************

void ApfCore::deleteLater()
{
    emit ApfCoreFinished();
}

//******************************************************************************

void ApfCore::protocolError()
{
    if (mVersionTransfered==false) // huummmm   Hacker???
    {
         close();
         deleteLater();
    }
}

//******************************************************************************

void ApfCore::data()
{
    if ( mApfCurrentOperation == 0 )
    {
        mApfCurrentOperation = new ApfNetworkOperation(ApfNetworkProtocol::OpNone,QString(""));
    }
    mApfNetworkProtocol->readData(mApfCurrentOperation);
}

//******************************************************************************

void ApfCore::exec()
{
    mMessage = QString(tr("command received: "));

    switch (mApfCurrentOperation->operation())
    {
        default:
             mMessage += QString( tr("unknown operation\n") );
             if (mVersionTransfered==false) // huummmm   Hacker???
             {
                close();
             }
        break;

        case  ApfNetworkProtocol::OpVersionRequest :
            execVersion(mApfCurrentOperation);
        break;

        case  ApfNetworkProtocol::OpDacLeft :
        case  ApfNetworkProtocol::OpDacRight :
            execSetDac(mApfCurrentOperation);
        break;

        case ApfNetworkProtocol::OpLed :
            execSetLed(mApfCurrentOperation);
        break;

        case ApfNetworkProtocol::OpLcd :
            execSetLcd(mApfCurrentOperation);
        break;
    }
    emit logText(mMessage);
    delete mApfCurrentOperation;
    mApfCurrentOperation = 0;
}

//******************************************************************************

void    ApfCore::execVersion(ApfNetworkOperation *)
{
    mMessage += QString(tr("Get version\n"));
    mVersionTransfered = true;
    ApfNetworkOperation VersionNetworkOperation(ApfNetworkProtocol::OpVersion,QString("1.0"));

    mApfNetworkProtocol->request(&VersionNetworkOperation);
	if (mApfDacController.isOpen() == false)
	{
		mMessage += QString(tr("Warning: The i2c bus is not accessible"));
	}
	else
	{
		mMessage += QString(tr("Info: The i2c bus is accessible"));
	}
}

//******************************************************************************

void    ApfCore::execSetDac(ApfNetworkOperation *ApfNetworkOperation)
{
    QString value = ApfNetworkOperation->arg(0);
    int val = value.toInt();

	if ( ApfNetworkOperation->operation() == ApfNetworkProtocol::OpDacLeft )
	{
		mMessage += QString(tr("Set left DAC operation : value = ")) + value;
		mApfDacController.setLeftDAC(val);
		emit setManometerValue(val * 10);
	}
	else
	{
		mMessage += QString(tr("Set right DAC operation : value = ")) + value;
		mApfDacController.setRightDAC(val);
		emit setThermometerValue(val * 3);
	}
}

//******************************************************************************

void ApfCore::execSetLed(ApfNetworkOperation *ApfNetworkOperation)
{
    QString arg = ApfNetworkOperation->arg(0);
    QString led = arg.left(1);
    QString state = arg.right(1);

    mMessage += QString(tr("Set LED operation : value = ")) + led + " " + state;
    emit setLedValue(led.toInt(), (state.toInt() ? true : false));
}

//******************************************************************************

void ApfCore::execSetLcd(ApfNetworkOperation *ApfNetworkOperation)
{
    QString text = ApfNetworkOperation->arg(0);

    mMessage += QString(tr("Set LCD text : ")) + text;
    emit updateText(text);
}

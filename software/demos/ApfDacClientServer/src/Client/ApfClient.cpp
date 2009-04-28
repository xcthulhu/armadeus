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
**	 ApfClient.cpp: Client for Apf server
**
**	author: carbure@users.sourceforge.net
*/


#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <QMessageBox>


//#include "ApfApplication.h"
#include "ApfClient.h"
#include "ApfNetworkOperation.h"


using namespace std;

//******************************************************************************

ApfClient::ApfClient( QString host, int port) :
mHost(host),mPort(port),mSocket(this)
{
 
}

//******************************************************************************

ApfClient::~ApfClient()
{
}

//******************************************************************************

void ApfClient::run()
{
	setupUi(this);
	rightDacControl->setRange(MIN_DAC,MAX_DAC);
	leftDacControl->setRange(MIN_DAC,MAX_DAC);
	mPreviousLeft = -1;
	mPreviousRight = -1;

	connect(&mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(displayError(QAbstractSocket::SocketError)));
	connect(&mSocket, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(&mSocket, SIGNAL(connected()), this, SLOT(initConnection()));
	connect(&mSocket, SIGNAL(disconnected()), this, SLOT(endPeerConnection()));
	connect(&mTimer, SIGNAL(timeout()), this, SLOT(timeOut()));
//	connect(rightDacControl, SIGNAL( valueChanged (int ) ), this, SLOT(setRightDac(int )));
//	connect(leftDacControl, SIGNAL( valueChanged (int ) ), this, SLOT(setLeftDac(int )));
	connect(cancelButton, SIGNAL( clicked ( ) ), this, SLOT(userClose( )));
	connect(greenLEDBox, SIGNAL(stateChanged(int)), this, SLOT(greenLEDChanged(int)));
	connect(redLEDBox, SIGNAL(stateChanged(int)), this, SLOT(redLEDChanged(int)));
	connect(lcdButton, SIGNAL(released()), this, SLOT(updateLCDText()));

	show();

	mSocket.connectToHost(mHost,mPort);
	//if ( mSocket.state() == QAbstractSocket::ConnectedState )
	mApfNetworkProtocol = new ApfNetworkProtocol( & mSocket );
}

//******************************************************************************

void ApfClient::readData()
{
	qDebug("Read data");
}

//******************************************************************************

void ApfClient::initConnection()
{
	qDebug("Connected");

	ApfNetworkOperation NetworkOperation(ApfNetworkProtocol::OpVersionRequest, QString(""));
	mApfNetworkProtocol->request( &NetworkOperation );
	mTimer.start(100);
}

//******************************************************************************

void ApfClient::endPeerConnection()
{
	qDebug("Disconnected");

	if (mSocket.state() == QAbstractSocket::UnconnectedState)
	{
		QMessageBox::information(this, tr("Apf Client"),
								tr("Disconnected by peer server \n"));
		close();
	}
	mTimer.stop();
}

//******************************************************************************

void ApfClient::userClose()
{
	qDebug("Disconnected");
	disconnect(rightDacControl, SIGNAL( valueChanged (int ) ), this, SLOT(setRightDac(int )));
	disconnect(leftDacControl, SIGNAL( valueChanged (int ) ), this, SLOT(setLeftDac(int )));
	disconnect(&mSocket, SIGNAL(disconnected()), this, SLOT(endPeerConnection()));
	close();
}

//******************************************************************************

void ApfClient::timeOut()
{
	int value;
	if ((cnt++%2) == 0)
	{
		value = rightDacControl->value();
		if (mPreviousRight != value) setRightDac(value);
		mPreviousRight = value;
	}
	else
	{
		value = leftDacControl->value();
		if (mPreviousLeft != value) setLeftDac(value);
		mPreviousLeft = value;
	}
}

//******************************************************************************

void ApfClient::setRightDac(int value)
{
	ApfNetworkOperation NetworkOperation(ApfNetworkProtocol::OpDacRight, QString("").sprintf("%d",value));
	mApfNetworkProtocol->request( &NetworkOperation );
	usleep(80000);
}

//******************************************************************************

void ApfClient::setLeftDac(int value)
{
	ApfNetworkOperation NetworkOperation(ApfNetworkProtocol::OpDacLeft, QString("").sprintf("%d",value));
	mApfNetworkProtocol->request( &NetworkOperation );
	usleep(80000);
}

//******************************************************************************

void ApfClient::greenLEDChanged(int aState)
{
	bool state;

	state = aState ? true : false;
	setLED(0, state);
}

void ApfClient::redLEDChanged(int aState)
{
	bool state;

	state = aState ? true : false;
	setLED(1, state);
}

void ApfClient::setLED(int id, int state)
{
	ApfNetworkOperation NetworkOperation(ApfNetworkProtocol::OpLed, QString("").sprintf("%d,%d", id, state));
	mApfNetworkProtocol->request(&NetworkOperation);
	usleep(80000);
}

//******************************************************************************

void ApfClient::updateLCDText(void)
{
	ApfNetworkOperation NetworkOperation(ApfNetworkProtocol::OpLcd, lcdLineEdit->text());
	mApfNetworkProtocol->request(&NetworkOperation);
	usleep(80000);
}

//******************************************************************************

void ApfClient::displayError(QAbstractSocket::SocketError err)
{
	switch (err)
	{
		case QAbstractSocket::RemoteHostClosedError:
			break;
		case QAbstractSocket::HostNotFoundError:
			QMessageBox::information(this, tr("Apf Client"),
										tr("The host was not found:\n Please check the "
										"host name and port settings."));
			break;
		case QAbstractSocket::ConnectionRefusedError:
			QMessageBox::information(this, tr("Apf Client"),
										tr("The connection was refused by the peer:\n"
										"Make sure the server is running, "
										"and check that the host name and port "
										"settings are correct."));
			break;
		default:
			QMessageBox::information(this, tr("Apf Client"),
										tr("The following error occurred: %1.")
									.arg(mSocket.errorString()));
	}
	close();
}

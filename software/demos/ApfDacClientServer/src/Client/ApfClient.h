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
**	 ApfClient.h: Client for Apf server
**
**	author: carbure@users.sourceforge.net
*/


#ifndef	__APFCLIENT_H_INCLUDED__
#define	__APFCLIENT_H_INCLUDED__

#include <QTcpSocket>
#include <QString>
#include <QTimer>

#include "ApfNetworkDefs.h"
#include "ApfNetworkProtocol.h"
#include "ui_Vmeters.h"

using namespace Ui;
using namespace ApfNetworkDefs;

class ApfClient : public QDialog, public  Vmeters
{
Q_OBJECT

public:

	ApfClient( QString host,int port=APF_SOCKET) ;

	virtual ~ApfClient();
	void run();

protected slots:    
	void readData();
	void displayError(QAbstractSocket::SocketError err);
	void setRightDac(int value);
	void setLeftDac(int value);
	void initConnection();
	void endPeerConnection();
	void userClose();
	void timeOut();
	void greenLEDChanged(int);
	void redLEDChanged(int);
	void setLED(int id, int state);
	void updateLCDText();

protected:

	QString mHost;
	int mPort;
	QTcpSocket mSocket;
	QTimer mTimer;
	int cnt;
	int mPreviousLeft,mPreviousRight;
	ApfNetworkProtocol *mApfNetworkProtocol;
};
#endif	// __APFCLIENT_H_INCLUDED__


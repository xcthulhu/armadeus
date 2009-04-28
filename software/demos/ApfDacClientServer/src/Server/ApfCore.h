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
**	 ApfCore.h: Core for Apf server
**
**	author: carbure@users.sourceforge.net
*/


#ifndef	__APFCORE_H_INCLUDED__
#define	__APFCORE_H_INCLUDED__

#include <QTcpSocket>
#include <QString>
#include <qtimer.h>

#include "ApfNetworkProtocol.h"
#include "ApfDacController.h"

class ApfCore : public QTcpSocket
{
    Q_OBJECT

public:

    ApfCore(QTcpSocket *clientConnection, QObject *parent=0, const char *name=0);

    virtual ~ApfCore();

signals:

//  output debug messages
    void logText( const QString& );
    void ApfCoreFinished();
    void setLedValue(int id, bool state);
    void setManometerValue(int id);
    void setThermometerValue(int id);
    void updateText(const QString &);

public slots:

    void exec();
    void data();
    void deleteLater();
    void protocolError();

private:

    QString mMessage;

protected:
    ApfNetworkProtocol        *mApfNetworkProtocol;
    ApfNetworkOperation       *mApfCurrentOperation;
    bool 	mVersionTransfered;

    void execVersion(ApfNetworkOperation*);
    void execSetDac(ApfNetworkOperation*);
    void execSetLed(ApfNetworkOperation*);
    void execSetLcd(ApfNetworkOperation*);

    QTcpSocket *mSocket;
    ApfDacController  mApfDacController;
};

#endif	// __APFCORE_H_INCLUDED__

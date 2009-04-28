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
**	 ApfNetworkProtocol.h
**
**	author: carbure@users.sourceforge.net
*/ 


#ifndef	__APFNETWORKPROTOCOL_H_INCLUDED__
#define	__APFNETWORKPROTOCOL_H_INCLUDED__

#include <QString>
#include <QByteArray>
#include <QObject>
#include <QIODevice>
#include <QDataStream>

#include "ApfNetworkDefs.h"

using namespace ApfNetworkDefs;
class   ApfNetworkOperation;

class ApfNetworkProtocol : public QObject
{
    Q_OBJECT

public:

    typedef enum Operation
    {
        OpNone=0,               // no operation (or must be set by protocol)
        OpVersion,              // get ATS version (compatibility)
        OpVersionRequest,       // request ATS version (compatibility)
        OpPing,                 // check if remote is responding
        OpDacLeft,              // change left DAC value
        OpDacRight,             // change right DAC value
        OpLed,                  // switch LEDs state
        OpLcd,                  // show text on LCD
        OpExitStatus            // send process exit status
    };

    typedef enum State
    {
        StWaiting = 0,
        StInProgress,
        StDone,
        StFailed,
        StStopped
    };

    typedef enum Error
    {
    // no error
        NoError = 0,
        ErrUnsupported,         // unsupported operation
        ErrIO                  // device error
    };


    ApfNetworkProtocol(QIODevice * d );

    virtual ~ApfNetworkProtocol();

    int request(ApfNetworkOperation *ApfNetworkOperation);
    int getIOError(); // get error from last  operation 
    int readData(ApfNetworkOperation *ApfNetworkOperation);

signals:
    void    finished();
    void    unexpectedCode();
    void    inProgress(int percentDone);

private:

protected:
    QIODevice *mDevice;
    QDataStream *mDataStream;

    int mIOError;
};

#endif	// __APFNETWORKPROTOCOL_H_INCLUDED__

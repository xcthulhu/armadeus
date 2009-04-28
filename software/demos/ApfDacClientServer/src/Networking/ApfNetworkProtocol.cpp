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
**	 ApfNetworkProtocol.cpp
**
**	author: carbure@users.sourceforge.net
*/

#include <iostream>
#include <unistd.h>

#include "ApfNetworkProtocol.h"
#include "ApfNetworkOperation.h"

//******************************************************************************

ApfNetworkProtocol::ApfNetworkProtocol(QIODevice * d ) :
mDevice(d)
{
    mDataStream = new QDataStream(d);
    mDataStream->setVersion(ApfNetworkDefs::DATASTREAM_RELEASE);     // Qt 3.1 and more
}

//******************************************************************************

ApfNetworkProtocol::~ApfNetworkProtocol()
{
    delete mDataStream;
}

//******************************************************************************

int ApfNetworkProtocol::request(ApfNetworkOperation *ApfNetworkOperation)
{
    Q_CHECK_PTR(ApfNetworkOperation);

    qint32 OP;
    Operation operation(ApfNetworkOperation->operation());
    OP = (qint32) operation;

    ApfNetworkOperation->setState(StInProgress);
    ApfNetworkOperation->setErrorCode(NoError);

    switch (operation)
    {
        default:
            ApfNetworkOperation->setErrorCode(ErrUnsupported);
            ApfNetworkOperation->setState(StFailed); 
            return 0;

        case OpVersionRequest:
            {
                *mDataStream << OP;
            }
            break;

        case OpDacLeft:
        case OpDacRight:
        case OpVersion:
        case OpLed:
        case OpLcd:
            {
                *mDataStream <<  OP;
                *mDataStream <<  ApfNetworkOperation->mArg[0].toAscii().data();
            }
            break;

        case OpNone:
        case OpPing:
            break;

        case OpExitStatus:
            return 0;
    }

    if (mDevice->waitForBytesWritten(10) == true)
    {
        ApfNetworkOperation->setErrorCode(ErrIO);
        ApfNetworkOperation->setState(StFailed);
    }
    else
    {
        ApfNetworkOperation->setState(StDone);
    }

    return (0);
}

//******************************************************************************

int ApfNetworkProtocol::readData(ApfNetworkOperation *ApfNetworkOperation)
{
    Q_CHECK_PTR(ApfNetworkOperation);

    qint32 OP;
    Operation expectedOperation(ApfNetworkOperation->operation());

    if (ApfNetworkOperation->mState != StInProgress)
    {
        *mDataStream >>  OP;
        if ( (expectedOperation   != (Operation) OP)  && (ApfNetworkOperation->mState != StWaiting)  )
        {
            qWarning("Expected operation and current operation are different: %d vs. %d", expectedOperation , (Operation)OP);
        }
    }
    else
    {
        OP = ApfNetworkOperation->mOperation;
    }
    ApfNetworkOperation->mOperation = (Operation) OP;
    ApfNetworkOperation->setErrorCode(NoError);

    switch (OP)
    {
        default:
            mDevice->readAll();
            ApfNetworkOperation->setState(StFailed);
            ApfNetworkOperation->setErrorCode(ErrUnsupported);
            qDebug("WARNING: Unexpected operation received");
            emit unexpectedCode();
            return -1;

        case OpDacLeft:
        case OpDacRight:
        case OpVersion:
        case OpLed:
        case OpLcd:
            {
                char * ptr =new char [mDevice->size()];
                *mDataStream >>  ptr;
                ApfNetworkOperation->setArg(0, QString (ptr));
                ApfNetworkOperation->setState(StDone);
                delete [] ptr;
            }
            break;

        case OpVersionRequest:
            ApfNetworkOperation->setState (StDone);
            break;

        case OpNone:
        case OpPing:
            break;
    }

    if (mDataStream->status () != QDataStream::Ok)
    {
        ApfNetworkOperation->setErrorCode(ErrIO);
        ApfNetworkOperation->setState(StFailed);  
        qDebug ("The status is not Ok: %d\n",mDataStream->status ());
        return -1;
    }
    else if (StDone == ApfNetworkOperation->state())
    {
        emit finished();
    }
    return (0);
}

//******************************************************************************

int ApfNetworkProtocol::getIOError()
{
    return mIOError;
}

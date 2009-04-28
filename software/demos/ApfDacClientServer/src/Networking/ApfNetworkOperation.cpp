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
**	 ApfNetworkOperation.cpp
**
**	author: carbure@users.sourceforge.net
*/ 

#include "ApfNetworkOperation.h"


//******************************************************************************

ApfNetworkOperation::ApfNetworkOperation( ApfNetworkProtocol::Operation operation,
        const QString &arg0, const QString &arg1,  const QString &arg2 )
{
    mOperation = operation;
    setArg(0, arg0);
    setArg(1, arg1);
    setArg(2, arg2);
    setState (ApfNetworkProtocol::StWaiting);
    setErrorCode(ApfNetworkProtocol::NoError);
}

ApfNetworkOperation::ApfNetworkOperation( ApfNetworkProtocol::Operation operation,
         const QByteArray &arg0, const QByteArray &arg1, const QByteArray &arg2 )
{
    mOperation = operation;
    setRawArg(0, arg0);
    setRawArg(1, arg1);
    setRawArg(2, arg2);
    setState (ApfNetworkProtocol::StWaiting);
    setErrorCode(ApfNetworkProtocol::NoError);
}

//*****************************************************************************

ApfNetworkOperation::~ApfNetworkOperation()
{
}

//******************************************************************************

void ApfNetworkOperation::setState( ApfNetworkProtocol::State state )
{
    mState =  state;
}

//******************************************************************************

void ApfNetworkOperation::setErrorCode( ApfNetworkProtocol::Error ec )
{
    mErrorCode = ec;
}

//******************************************************************************

void ApfNetworkOperation::setArg( int num, const QString &arg )
{
    if (num  < 3)
    {
        mArg[num] = arg;
    }
}

//******************************************************************************

void ApfNetworkOperation::setRawArg( int num, const QByteArray &arg )
{
    if (num  < 3)
    {
        mRawArg[num] = arg;
        mRawArg[num].detach();
    }
}

//******************************************************************************

void ApfNetworkOperation::appendRawArg( int num, const QByteArray &arg )
{
    if (num  < 3)
    {
        if  (true == mRawArg[num].isNull ())
        {
            setRawArg(num,arg);
            mRawArg[num].detach();
        }
        else
        {
            int previousSize;
            previousSize = mRawArg[num].size();
            mRawArg[num].resize(previousSize + arg.size());
            memcpy (mRawArg[num].data()+ previousSize, arg.data(),  arg.size());
        }
    }
}

//******************************************************************************

ApfNetworkProtocol::Operation ApfNetworkOperation::operation() const
{
    return mOperation;
}

//******************************************************************************

QString ApfNetworkOperation::arg( int num ) const
{
    if (num  > 2)
    {
        num = 0;
    }
    return mArg[num];
}

//******************************************************************************

QByteArray ApfNetworkOperation::rawArg( int num ) const
{
    if (num  > 2)
    {
        num = 0;
    }
    return mRawArg[num];
}
//******************************************************************************

ApfNetworkProtocol::State ApfNetworkOperation::state() const
{
    return mState;
}

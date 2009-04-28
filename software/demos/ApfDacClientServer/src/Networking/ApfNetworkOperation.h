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
**	 ApfNetworkOperation.h
**
**	author: carbure@users.sourceforge.net
*/ 


#ifndef	__APFNETWORKOPERATION_H_INCLUDED__
#define	__APFNETWORKOPERATION_H_INCLUDED__

#include <QString>
#include <QByteArray>

#include "ApfNetworkProtocol.h"


class ApfNetworkOperation
{

public:

    ApfNetworkOperation( ApfNetworkProtocol::Operation operation,
        const QString &arg0, const QString &arg1=0,  const QString &arg2=0 );
    ApfNetworkOperation( ApfNetworkProtocol::Operation operation,
        const QByteArray &arg0, const QByteArray &arg1=0, const QByteArray &arg2=0 );

    virtual ~ApfNetworkOperation();

    void setState( ApfNetworkProtocol::State state );
    void setErrorCode( ApfNetworkProtocol::Error ec );
    void setArg( int num, const QString &arg );
    void setRawArg( int num, const QByteArray &arg );
    void appendRawArg( int num, const QByteArray &arg );

    ApfNetworkProtocol::Operation operation() const;
    ApfNetworkProtocol::State state() const;
    QString arg( int num ) const;
    QByteArray rawArg( int num ) const;

private:


protected:
    friend class ApfNetworkProtocol;

    ApfNetworkProtocol::Operation mOperation;
    ApfNetworkProtocol::State mState;
    ApfNetworkProtocol::Error  mErrorCode;
    QString mArg[3];
    QByteArray mRawArg[3];

};
#endif	// __APFNETWORKOPERATION_H_INCLUDED__


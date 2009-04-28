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
**	 ApfNetworkDefs.h
**
**	author: carbure@users.sourceforge.net
*/ 


#ifndef	__APFNETWORKDEFS_H_INCLUDED__
#define	__APFNETWORKDEFS_H_INCLUDED__


namespace ApfNetworkDefs
{
    // The Dynamic and/or Private Ports are those from 49152 through 65535
    static const unsigned int APF_SOCKET = 50154; //why not ?
    static const unsigned int DATASTREAM_RELEASE = 5;
    static const  int MIN_DAC = 0;
    static const  int MAX_DAC = 32;
}

#endif	// __APFNETWORKDEFS_H_INCLUDED__

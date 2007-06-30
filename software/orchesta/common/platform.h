/*
**	THE ARMADEUS PROJECT
**
**  Copyright (2007)  The source forge armadeus project team
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
**	  platform.h: FPGA platform wrapper
**
**	author: thom25@users.sourceforge.net
*/
#ifndef PLATFORM_H
#define PLATFORM_H

#include "xmldoc.h"

class Platform : public XMLNodeWrapper
{
	public:
		Platform():XMLNodeWrapper(){};
		virtual ~Platform(){};

		QString getFPGAType()       { return getAttribute("fpga", "type"); };
		QString getFPGASpeed()      { return getAttribute("fpga", "speed"); };
		QString getFPGAPackage()    { return getAttribute("fpga", "package"); };
        QString getFPGAFamily()     { return getAttribute("fpga", "family"); };
		QString getBoardType()      { return getAttribute("board", "type"); };
		QString getDescription()    { return getCDATA("description"); };
        QString getFullFPGAType();
};

#endif //PLATFORM_H

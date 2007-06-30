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
**	  toolchain.h: FPGA toolchain wrapper
**
**	author: thom25@users.sourceforge.net
*/
#ifndef TOOLCHAIN_H
#define TOOLCHAIN_H

#include "xmldoc.h"
#include "platform.h"

class Project;

class Toolchain : public XMLNodeWrapper
{
    protected:
        Project *project;
       
       virtual void checkErrors() = 0;
       virtual bool copyIps();

	public:
		Toolchain():XMLNodeWrapper(){project=NULL;};
		virtual ~Toolchain(){};

		virtual QString getType()       { return getAttribute("infos", "type"); };
		virtual QStringList getCommandLines();
        virtual bool execute() = 0;
        virtual void initialize(QDomNode aNode, Project* aProject);
        
};

class XilinxToolchain : public Toolchain
{
    protected:
        bool createXilinxScript();

       virtual void checkErrors();

    public:
		XilinxToolchain():Toolchain(){};
		virtual ~XilinxToolchain(){};

        virtual bool execute();
        virtual bool buildXilinxPrj();
};




#endif //TOOLCHAIN_H

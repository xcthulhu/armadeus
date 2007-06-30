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
**	 armadeus_ready.h: armadeus_ready utilities
**
**	author: thom25@users.sourceforge.net
*/
#ifndef ARMADEUS_READY_H
#define ARMADEUS_READY_H

#include <qstring>
#include <qdir>

#define ARMADEUS_READY_FOLDER "/armadeus_ready/"

class ArmadeusReadyIpFinder 
{
    protected:
        QString armadeusReadyPath;

        void getNextIpAbsoluteNames(QFileInfoList *ipNamesList, QDir dir);       
        QFileInfoList getAllIpAbsoluteNames();
        
	public:
		ArmadeusReadyIpFinder(){};
		virtual ~ArmadeusReadyIpFinder(){};

        virtual void setPath(QString path);
		virtual QStringList getAllIpNames();
        virtual QString getIpAbsoluteFilePath( QString fileName );
};


#endif //ARMADEUS_READY_H

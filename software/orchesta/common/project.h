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
**	  project.h: FPGA project manager
**
**	author: thom25@users.sourceforge.net
*/
#ifndef PROJECT_H
#define PROJECT_H

#include "xmldoc.h"
#include "ip.h"
#include "platform.h"
#include "toolchain.h"


class Project
{
	protected:
        QString docFileName;
		QDomDocument doc;
        
        virtual QDomNode findIpNode(QString anIpName);
        virtual QDomNode findPlatformNode();
        virtual QDomNode findToolchainNode();
        virtual bool getDocFromXmlFile(QString fileName, QDomDocument *tempDoc);
        virtual QMap<QString, QString> Project::getProjectVariables();

    public:
		Project();
		virtual ~Project();
	
        virtual bool removePlatform();
        virtual bool removeIp(QString anIpName);
        virtual bool addIp(QString ipName, QString ipFileName);
        virtual bool addToolchain(QString toolchainTemplate);
        virtual bool addPlatform(QString platformFileName);

        virtual bool get(Platform *result);
        virtual bool get(Toolchain *result);
        virtual bool get(QString anIpName, Ip *result);
        virtual QString getName();
        virtual QString getFileName() {return docFileName;};
        
        virtual bool modifyIpBaseAddress(QString ipName, QString baseAddress);
        virtual bool computeIpBaseAddresses();
        virtual bool checkFreeRegisterAddressRange(QString address);
		
        virtual bool setToolchain(const QString type="Xilinx");
        
        virtual QStringList getIpNames();
        virtual bool initialize(QString aFileName);
        virtual bool create(QString aFileName);
        virtual bool saveToFile(QString aFileName="");
        virtual void resolveProjectVariables( QStringList *commandList );
        virtual void resolveProjectVariables( QString *command );
        
};

#endif // PROJECT_H

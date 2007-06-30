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
**	 ip.h: FGPA ip wrapper
**
**	author: thom25@users.sourceforge.net
*/
#ifndef IP_H
#define IP_H

#include <qstringList>
#include <qrect>
#include "xmldoc.h"

class AddressRange
{
    protected:
        int top;
        int bottom;
    
    public:
        AddressRange();
        AddressRange(int byteSize);
        AddressRange(int start, int length);
        virtual ~AddressRange(){};
        
        virtual void adjustStartAddress( int address );
        virtual void adjustEndAddress( int address );
        virtual void merge( AddressRange range );
        virtual bool intersects( AddressRange range );
		virtual int start();
		virtual int end();
};






class Ip : public XMLNodeWrapper
{
	protected:
    
        QStringList getNameList(QString nodeName);
        
    public:
		Ip():XMLNodeWrapper(){};
		virtual ~Ip(){};
	
		QString getReference() {return xmlNode.toElement().attribute("name");};
		QString getDescription() {return getCDATA("description");};
		QString getVersion() {return xmlNode.toElement().attribute("version");};
        QString getName() { return xmlNode.toElement().attribute("user_name");};
    
        QString getGenericAttribute(QString genericName, QString attributeName)
                                        {return getAttribute("generics", genericName, attributeName);};
        bool setGenericAttributeValue(QString genericName, QString attribute, QString attributeValue);
        QStringList getGenericNameList() {return getNameList("generics");};

        QStringList getRegistersNameList() {return getNameList("registers");};
        QString getRegisterAttribute(QString registerName, QString attributeName)
                                        {return getAttribute("registers", registerName, attributeName);};

        QString getHDLAttribute(QString HDLName, QString attributeName)
                                        {return getAttribute("hdl_files",HDLName, attributeName);};
        QStringList getHDLNameList() {return getNameList("hdl_files");};
        QString getHDLTopName();
        
        QString getBaseAddress();
        void setBaseAddress(QString address);
        QString getInterrupt();
        void setInterrupt(QString interrupt);
        
        bool isRegisterAddressConflict(QString registerAddress);
        AddressRange getRegisterAddressRange();
    
};

#endif // IP_H
/*




class Ip : public XMLDoc
{
	protected:
        QStringList getNameList(QString nodeName);
//        virtual QString getAttribute(QString startNodeName, QString nodeName, QString attributeName);
        QDomNode getGenericsNode(){return getTopNode().firstChildElement("generics");}
        QDomNode getHDLsNode(){return getTopNode().firstChildElement("hdl_files");}
        QDomNode getRegistersNode(){return getTopNode().firstChildElement("registers");}
        
    public:
		Ip();
		virtual ~Ip();
	
		QString getReference() {return getAttribute(QDomNode(), QString("ip"), QString("ref"));};
		QString getDescription() {return getCDATA(doc.documentElement(), "description");};
		QString getVersion() {return getAttribute(QDomNode(), "ip", "version");};
        QString getName() {return getAttribute(QDomNode(), "ip", "name");};
    
        QString getGenericAttribute(QString genericName, QString attributeName)
                                        {return getAttribute(getGenericsNode(), genericName, attributeName);};
        bool setGenericAttributeValue(QString genericName, QString attribute, QString attributeValue);
        QStringList getGenericNameList() {return getNameList("generics");};

        QStringList getRegistersNameList() {return getNameList("registers");};
        QString getRegisterAttribute(QString registerName, QString attributeName)
                                        {return getAttribute(getRegistersNode(), registerName, attributeName);};

        QString getHDLAttribute(QString HDLName, QString attributeName)
                                        {return getAttribute(getHDLsNode(), HDLName, attributeName);};
        QStringList getHDLNameList() {return getNameList("hdl_files");};
        QString getHDLTopName();
        
        QString getBaseAddress();
        void setBaseAddress(QString address);
        
        bool isRegisterAddressConflict(QString registerAddress);
        AddressRange getRegisterAddressRange();
    
        virtual bool initialize(QString aFileName, QString anIpName); // initialize the ip object with the informations contained in the xml file
        virtual bool initialize(QDomNode *ipNode){ return XMLDoc::initialize(ipNode); };
};

#endif // IP_H
*/

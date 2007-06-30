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
**	  xmldoc.h: XML document base class
**
**	author: thom25@users.sourceforge.net
*/
#ifndef XMLDOC_H
#define XMLDOC_H

#include <qstringlist>
#include <qfile>
#include <qdomdocument>

        

class XMLNodeWrapper
{
	protected:
		QDomNode xmlNode;
        virtual QString getCDATA( QString elementName );
        
	public:
		XMLNodeWrapper();
		virtual ~XMLNodeWrapper(){};
	
        virtual void initialize(QDomNode aNode);
        virtual QString getAttribute( QString elementName, QString attributeName );        
        virtual QString getAttribute( QString subNodeName, QString elementName, QString attributeName ); 
        virtual bool setAttributeValue( QString subNodeName, QString elementName, 
                                        QString attributeName, QString attributeValue);
        virtual bool setAttributeValue( QString elementName, QString attributeName, QString attributeValue);
        virtual QString toString(); 

};

#endif // XMLDOC_H

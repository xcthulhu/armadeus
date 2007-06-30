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
**	  xmldoc.cpp: XML document base class
**
**	author: thom25@users.sourceforge.net
*/
#include "xmldoc.h"
#include <QTextStream>

XMLNodeWrapper::XMLNodeWrapper(){
} 
	
QString XMLNodeWrapper::getAttribute( QString subNodeName, QString elementName, QString attributeName ){
    QDomElement subNode = xmlNode.firstChildElement(subNodeName);
    if( !subNode.isNull() ){
        QDomElement element = subNode.firstChildElement(elementName);
        if( !element.isNull() )
            return element.attribute(attributeName);
        else{  // not a tag name. try with attribute name
            QDomNodeList elementList = subNode.childNodes();
            for( int i =0; i<elementList.size(); i++ ){
                if( elementList.at(i).toElement().attribute("name") == elementName ){
                    return elementList.at(i).toElement().attribute(attributeName);
                }
            }
        }
    }
    return QString();
}        
   
QString XMLNodeWrapper::getAttribute( QString elementName, QString attributeName ){
    QDomElement e;
    e = xmlNode.firstChildElement(elementName);
    if( !e.isNull() ){
        return e.attribute(attributeName);
    }
    else {  // not a tag name... look at a node with attribute elementName
        QDomNodeList elementList = xmlNode.childNodes();
        for( int i =0; i<elementList.size(); i++ ){
            //    printf("getAttribute %s", elementList.at(i).toElement().attribute("name").toLatin1().constData());
            if( elementList.at(i).toElement().attribute("name") == elementName ){
                return elementList.at(i).toElement().attribute(attributeName);
            }
        }
    }
    return "";
}

bool XMLNodeWrapper::setAttributeValue(QString elementName, QString attributeName, QString attributeValue)
{
    QDomElement element = xmlNode.firstChildElement(elementName);
    if( !element.isNull() ){
        element.setAttribute(attributeName, attributeValue);
        return true;
    }
    return false;    
}

bool XMLNodeWrapper::setAttributeValue(QString subNodeName, QString elementName, QString attributeName, QString attributeValue)
{
    QDomNode subNode = xmlNode.firstChildElement(subNodeName);
    if( !subNode.isNull() ){
        QDomElement element = subNode.firstChildElement(elementName);
        if( !element.isNull() ){
            element.setAttribute(attributeName, attributeValue);
            return true;
        }
    }
    return false;    
}

QString XMLNodeWrapper::getCDATA( QString elementName ){
    QDomElement e;
    e = xmlNode.firstChildElement(elementName);
    if( !e.isNull() ){
        if( ! e.firstChild().toCDATASection().isNull() )
            return e.firstChild().toCDATASection().data();
    }
    return "";    
}

void XMLNodeWrapper::initialize(QDomNode aNode){
    xmlNode = aNode;
}

QString XMLNodeWrapper::toString(){ 
    QString result;
    QTextStream stream(&result);
    xmlNode.save(stream,4);
    return result;
}


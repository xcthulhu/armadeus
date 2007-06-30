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
**	 ip.cpp: FGPA ip wrapper
**
**	author: thom25@users.sourceforge.net
*/
#include <qstringlist>
#include <qfile>
#include <qdomdocument>
#include <qbytearray>
#include <qtextstream>
#include <qregexpvalidator>
#include "ip.h"

AddressRange::AddressRange():top(0),bottom(0){};
AddressRange::AddressRange(int byteSize):top(0), bottom(byteSize-1){};
AddressRange::AddressRange(int start, int length):top(start), bottom(start+length-1){};
        
void AddressRange::adjustStartAddress(int address){ top += address; bottom += address; }
void AddressRange::adjustEndAddress(int address){ bottom += address; }

void AddressRange::merge(AddressRange range){ 
//    printf("address Range: %d, %d\n", range.start(), range.end());
    if( range.end() > bottom )
        bottom = range.end();
    if( range.start() < top )
        top = range.start();
}
bool AddressRange::intersects(AddressRange range){
    if( (range.start() >= top) || (range.end() <= bottom) )
        return true;
    return false; 
}
int AddressRange::start(){ return top; }
int AddressRange::end(){ return bottom; }




bool Ip::setGenericAttributeValue(QString genericName, QString attributeName, QString attributeValue)
{
    QString filterString = getAttribute("generics", genericName, "valid");
    if( filterString != "" ){ // if filter present
        int pos=0;
        QRegExpValidator validator(QRegExp(filterString), NULL);
        if( validator.validate(attributeValue, pos) == QValidator::Acceptable ){
            setAttributeValue("generics", genericName, attributeName, attributeValue);
            return true;
        }
    }
    else {
        return setAttributeValue("generics", genericName, attributeName, attributeValue);
    }
    return false;
}

QString Ip::getHDLTopName(){
    QDomElement hdls = xmlNode.firstChildElement("hdl_files");
    QDomNodeList hdlList = hdls.childNodes();
    for( int i =0; i<hdlList.size(); i++ ){
        if( hdlList.at(i).toElement().attribute("istop") == "1" )
            return hdlList.at(i).toElement().attribute("name");
    }
    return QString();
}


QStringList Ip::getNameList(QString nodeName){
    QStringList result;
    QDomElement generics = xmlNode.firstChildElement(nodeName);
    QDomNodeList genericList = generics.childNodes();
    for( int i =0; i<genericList.size(); i++ ){
        if( genericList.at(i).isElement() ) // avoid non Element
            result += genericList.at(i).toElement().attribute("name");
    }
    return result;
}

QString Ip::getBaseAddress(){
    return xmlNode.firstChildElement("base_address").attribute("value"); 
}

void Ip::setBaseAddress(QString address){
    QDomElement base_address = xmlNode.firstChildElement("base_address");
    base_address.setAttribute("value",address);
}

QString Ip::getInterrupt(){
    return xmlNode.firstChildElement("interrupt").attribute("value"); 
}

void Ip::setInterrupt(QString interrupt){
    QDomElement interruptElement = xmlNode.firstChildElement("interrupt");
    interruptElement.setAttribute("value",interrupt);
}



AddressRange Ip::getRegisterAddressRange(){
    AddressRange range;
    QStringList registerNameList =  getRegistersNameList();
    for( int i=0; i<registerNameList.count(); i++ ){
        QString offset = getRegisterAttribute(registerNameList.at(i), "offset");
        QString width = getRegisterAttribute(registerNameList.at(i), "width");
        bool ok;
        
        if( width.toInt() )
            range.merge(AddressRange(offset.remove("0X").toInt(&ok,16),width.toInt()/8));
    }
    if( getBaseAddress() != "" ){
        range.adjustStartAddress(getBaseAddress().toInt());
    }
//    printf("address Range: %d, %d\n", range.start(), range.end());
    return range;
}

bool Ip::isRegisterAddressConflict(QString registerAddress){
    return getRegisterAddressRange().intersects(AddressRange(registerAddress.toInt()));
}


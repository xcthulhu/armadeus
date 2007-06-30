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
**	  project.cpp: FPGA project manager
**
**	author: thom25@users.sourceforge.net
*/
#include "project.h"
#include "ip.h"
#include "platform.h"
#include <qfileinfo>
#include <qtextstream>
#include <qmap>
#include <qcoreapplication>
 
Project::Project(){
    docFileName = "";
}

Project::~Project(){}

QMap<QString, QString> Project::getProjectVariables(){
    QMap<QString, QString> result;
    Platform plt;
    get(&plt);
    result.insert("LT", "<"); // for <
    result.insert("GT", ">"); // for >
    result.insert("PROJECTNAME", getName());
    result.insert("FPGAFULLTYPE", plt.getFullFPGAType());
    
    return result;
}

QDomNode Project::findIpNode(QString anIpName) {
    QDomNodeList nodeList = doc.documentElement().elementsByTagName("ip");
    for( int i=0; i<nodeList.size(); i++ ){
  		QDomElement e = nodeList.at(i).toElement(); // try to convert the node to an element.
		if(!e.isNull()) {
            if( e.attribute("user_name") == anIpName ){
                return nodeList.at(i);
            }
        }
    }
    return QDomNode();
}

QDomNode Project::findPlatformNode() {
	QDomElement docElem = doc.documentElement();
    QDomNodeList nodeList = docElem.elementsByTagName("platform");
    if( !nodeList.isEmpty() )
        return nodeList.at(0);
    return QDomNode();
}


QDomNode Project::findToolchainNode() {
	QDomElement docElem = doc.documentElement();
    QDomNodeList nodeList = docElem.elementsByTagName("toolchain");
    if( !nodeList.isEmpty() )
        return nodeList.at(0);
    return QDomNode();
}

bool Project::addIp(QString ipName, QString ipFileName) {
    if( findIpNode(ipName).isNull() ){ // test whether the ipname already exists or not
        QDomDocument tempDoc;
        if( getDocFromXmlFile(ipFileName, &tempDoc) ){
            QDomElement ipNode = tempDoc.documentElement();
            
            QDomElement interrupt = tempDoc.createElement("interrupt");
            interrupt.setAttribute("value","");
            ipNode.appendChild(interrupt);
            QDomElement base_address = tempDoc.createElement("base_address");
            base_address.setAttribute("value","");
            ipNode.appendChild(base_address);
            ipNode.setAttribute("user_name",ipName);
            doc.documentElement().appendChild( doc.importNode( tempDoc.documentElement(), true ));
            
            return true;
        }
        printf("unable to read Ip xml file\n");     
        return false;  
    }
    printf("Ip already exists\n");     
    return false;
}

bool Project::removeIp(QString anIpName){
    QDomNode node = findIpNode(anIpName);
    if( !node.isNull() ){
        QDomNode parent = node.parentNode();
        if( !parent.isNull()){
            parent.removeChild(node);
            return true;
        }
    }
    printf("no Ip found\n");
    return false;
}

bool Project::removePlatform(){
    QDomNode pltNode = findPlatformNode();
    if( !pltNode.isNull() ){
        if( !doc.documentElement().removeChild(pltNode).isNull() )
            return true;
    }  
    printf("no platform\n");
    return false;
}


bool Project::create(QString aFileName){
    docFileName = aFileName;
    QDomElement root = doc.createElement("project");
    doc.appendChild(root);
	return true;
}

bool Project::initialize(QString aFileName){
    docFileName = aFileName;
	QFile file(docFileName);
	if (!file.open(QIODevice::ReadOnly)){
        printf("unable to open project file\n");
        return false;
    }
	if (!doc.setContent(&file)) {
        printf("unable to read project file\n");
        return false;
	}
	file.close();
	return true;
}

bool Project::addPlatform(QString platformFileName){
    if( findPlatformNode().isNull() ){
        QDomDocument tempDoc;
        if( getDocFromXmlFile(platformFileName, &tempDoc) ){
            doc.documentElement().appendChild( doc.importNode( tempDoc.documentElement(), true ));
            return true;
        }
        return false;
    }
    printf("platform already exists\n");
    return false;
}

bool Project::addToolchain(QString toolchainTemplate){
    QDomNode toolNode = findToolchainNode();
    if( !toolNode.isNull() ){
        if( doc.documentElement().removeChild(toolNode).isNull() )
            return false;
    }  
    QDomDocument tempDoc;
    printf("%s\n",(QCoreApplication::applicationDirPath()+"/"+toolchainTemplate).toLatin1().constData());
    if( getDocFromXmlFile(QCoreApplication::applicationDirPath()+"/"+toolchainTemplate, &tempDoc) ){
        doc.documentElement().appendChild( doc.importNode( tempDoc.documentElement(), true ));
        return true;
    }
    return false;
}



bool Project::getDocFromXmlFile(QString fileName, QDomDocument *tempDoc){
    QFile tempFile(fileName);
    if (!tempFile.open(QIODevice::ReadOnly)){
        printf("unable to open xml file %s\n",fileName.toLatin1().constData());    
        return false;
    }
    if (!tempDoc->setContent(&tempFile)) {
        printf("unable to set content of xml doc\n");    
        tempFile.close();
        return false;
    }
    tempFile.close();
    return true;
}


QString Project::getName(){
    return QFileInfo(getFileName()).baseName();
}

QStringList Project::getIpNames(){
    QStringList result;
    
    QDomNodeList nodeList = doc.documentElement().elementsByTagName("ip");
    for( int i=0; i<nodeList.size(); i++ ){
  		QDomElement e = nodeList.at(i).toElement(); // try to convert the node to an element.
  		if(!e.isNull())
            result.append(e.attribute("user_name"));
    }
    return result;
}

bool Project::get(Toolchain *result){
    QDomNode node = findToolchainNode();
    if( !node.isNull() ){
        result->initialize(node, this);
        return true;
    }    
    return false;
}

bool Project::get(Platform *result){
    QDomNode node = findPlatformNode();
    if( !node.isNull() ){
        result->initialize(node);
        return true;
    }    
    return false;
}

bool Project::get(QString anIpName, Ip *result){
    QDomNode node = findIpNode(anIpName);
    if( !node.isNull() ){
        result->initialize(node);
        return true;
    }    
    return false;
}

bool Project::checkFreeRegisterAddressRange(QString address){
    Ip ip;
    QStringList ipList = getIpNames();
    for( int i=0; i<ipList.count(); i++ ){
        get(ipList.at(i), &ip);
        if( ip.isRegisterAddressConflict(address) )
            return false;
    }
    return true;
}

bool Project::modifyIpBaseAddress(QString ipName, QString baseAddress){
    Ip ip;
    if( get(ipName, &ip) ){
        if( checkFreeRegisterAddressRange(baseAddress) ){
            ip.setBaseAddress(baseAddress);
            return true;
        }
    }
    return false;
}

bool Project::computeIpBaseAddresses(){
    Ip ip;
    QStringList ipList = getIpNames();
    if( ipList.count() > 0 ){
        get(ipList.at(0), &ip);
        
        // fix first IP address if not set
        if( ip.getBaseAddress() == "" )
            ip.setBaseAddress(QString::number(0));
        
        int previousEndAddress = ip.getRegisterAddressRange().end() + 1;
        printf("set ip %s base address %s \n", ip.getName().toLatin1().constData(), ip.getBaseAddress().toLatin1().constData());
        for( int i=1; i<ipList.count(); i++ ){
            get(ipList.at(i), &ip);
            ip.setBaseAddress(QString::number(previousEndAddress));
            printf("set ip %s base address %s \n", ip.getName().toLatin1().constData(), ip.getBaseAddress().toLatin1().constData());
            int endAddress = ip.getRegisterAddressRange().end();
            previousEndAddress = endAddress + 1;
        }
    }
    else
        printf("Warning no IP\n");
    return true;
}

bool Project::saveToFile(QString aFileName){
    QString finalFileName = aFileName;
    if( aFileName == "" )
        finalFileName = docFileName;
    if( !finalFileName.contains(".pro") )
        finalFileName.append(".pro");
        
	QFile destFile(finalFileName);
	if (!destFile.open(QIODevice::ReadWrite | QIODevice::Truncate)){
			return false;
	};

    QString xmlContent = doc.toString();
    QTextStream out(&destFile);
    out << xmlContent;
    return true;
}

bool Project::setToolchain(const QString type){
    if( type.compare("xilinx") == 0 ){
        return addToolchain("xilinx_toolchain.xml");
    } 
    else {
        printf("Error Altera projects not supported\n");
    }
    return false;
}

void Project::resolveProjectVariables( QString *command ){
    QMap<QString, QString> variables = getProjectVariables();
    QMapIterator<QString, QString> it(variables);

    while (it.hasNext()) {
        it.next();
        command->replace("$"+it.key(),it.value());  // pour chaque command remplacer les variables de la map
    }
}


void Project::resolveProjectVariables( QStringList *commandList ){
    QMap<QString, QString> variables = getProjectVariables();
    QMapIterator<QString, QString> it(variables);

    for( int i=0; i<commandList->size(); i++ ){
        while (it.hasNext()) {
            it.next();
 //           printf("key: %s, valeu: %s \n",it.key().toLatin1().constData(), it.value().toLatin1().constData());
            commandList->replaceInStrings("$"+it.key(),it.value());  // pour chaque command remplacer les variables de la map
        }
    }
}




/*
QString Project::serializeIp(QString anIpName){
    Ip *ip = findIp( anIpName );
    if( ip ) 
        return ip->serialize();
    return "";
}*/



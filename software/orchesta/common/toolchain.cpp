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
**	  toolchain.cpp: FPGA toolchain wrapper
**
**	author: thom25@users.sourceforge.net
*/
#include "toolchain.h"
#include "project.h"
#include "ip.h"
#include "armadeus_ready.h"
#include <qtextstream>
#include <qprocess>
#include <qfileinfo>
#include <qdir>
#include <qcoreapplication>

void Toolchain::initialize(QDomNode aNode, Project* aProject){
    XMLNodeWrapper::initialize(aNode);
    project = aProject;
}

QStringList Toolchain::getCommandLines(){
    QStringList result;
    QDomElement flow = xmlNode.firstChildElement("flow");
    QDomNodeList commandList = flow.childNodes();
    for( int i =0; i<commandList.size(); i++ ){
        if( commandList.at(i).isElement() ) { // avoid non Element
            QString temp =  commandList.at(i).toElement().attribute("name") + " " + 
                            commandList.at(i).toElement().attribute("options");
            result += temp;
//            printf("command list: %s\n", temp.toLatin1().constData());
        }
    }
    return result;
}

bool Toolchain::copyIps(){

    ArmadeusReadyIpFinder finder;
    finder.setPath(QCoreApplication::applicationDirPath()+ARMADEUS_READY_FOLDER);
    QStringList ipFileNameList = finder.getAllIpNames();
    for( int i=0; i<ipFileNameList.count(); i++ ){
        QString ipDirectory = QFileInfo(finder.getIpAbsoluteFilePath(ipFileNameList.at(i))).absolutePath(); 
        QProcess process;
//        printf("cp -r %s/*.* .\n", ipDirectory.toLatin1().constData());
//        process.start("cp -r "+ipDirectory+"/*.* .");    
//        printf("xcopy  %s . /E\n", ipDirectory.replace(QString("/"), QString("\\")).toLatin1().constData());
        process.start("xcopy "+ipDirectory.replace(QString("/"), QString("\\"))+" . /E /Y");  
        process.waitForFinished();        
    }
    return true;
}




bool XilinxToolchain::buildXilinxPrj(){
    Ip currentIp;
    QStringList result;
    QString projectName = project->getFileName();
    QStringList ipNameList = project->getIpNames();

    // create hdl top file list
    for( int i=0; i<ipNameList.count(); i++ ){
        if( project->get(ipNameList.at(i), &currentIp ) ){
//            printf("IP: %s\n", currentIp.getName().toLatin1().constData());
            QString topIpFileName = currentIp.getHDLTopName();
            QStringList HDLlist = currentIp.getHDLNameList();
            int pos = HDLlist.indexOf(topIpFileName);
            if( pos != -1)
                HDLlist.removeAt(pos);
            HDLlist.prepend(topIpFileName); // reorganize in a proper order
            if( topIpFileName != "" ){
                for( int j=0; j<HDLlist.size(); j++ ){
                    if( !result.filter(HDLlist.at(j)).size() ) // if not already present in the list
                    {
                        if( QFileInfo(HDLlist.at(j)).suffix() == "vhd" ) // check whether it is a vhdl or a verilog file
                            result+= "vhdl work \""+HDLlist.at(j)+'"';
                        else if( QFileInfo(HDLlist.at(j)).suffix() == "v" )
                            result+= "verilog work \""+ HDLlist.at(j) + '"';
                    }
                }
            }
            else
                printf("Warning no top file for IP: %s", currentIp.getName().toLatin1().constData());
        }
    }
    
    // create prj file
	QFile file(project->getName()+".prj");
	if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)){
        printf("can not create file %s\n", projectName.toLatin1().constData());
		return false;
    }

    QTextStream out(&file);
    for( int i=0; i<result.size(); i++ )
        out << result.at(i) + "\n" ;
	file.close();
    return true;
}

void XilinxToolchain::checkErrors()
{
    QFile file(project->getName()+".log");
    if (!file.open(QIODevice::ReadOnly)){
        printf("Error: unable to open project log file\n"); 
    } else {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if(line.contains("ERROR:"))
                printf("%s\n", line.toLatin1().constData());
        }
    }
}

bool XilinxToolchain::execute(){
    if( project ){
        if( !copyIps() )
            return false;
        QProcess builder;
        builder.setProcessChannelMode(QProcess::MergedChannels);
        
        // create .xstprojnav.tmp
        QDir tempDir(".");
        tempDir.mkdir(QString("xst"));
        tempDir.mkdir(QString("xst/projnav.tmp"));
        
        QDomElement flow = xmlNode.firstChildElement("xst_script");
        QDomNodeList xst_script = flow.childNodes();
        QString temp;
        for( int i =0; i<xst_script.size(); i++ ){
            temp +=  xst_script.at(i).toElement().attribute("name") + " " + 
                            xst_script.at(i).toElement().attribute("options") + "\n";
        }
        project->resolveProjectVariables(&temp);
   
        // create prj file
    	QFile file(project->getName()+".xst");
    	if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    		return false;
        QTextStream out(&file);
        out << temp;
    	file.close();
 
        QStringList commandList = getCommandLines();
        project->resolveProjectVariables(&commandList);

        for( int i=0; i<commandList.size(); i++ ){
//            printf("%s\n", commandList.at(i).toLatin1().constData());
            builder.start(commandList.at(i));
            builder.waitForFinished();
       }
       
       // check errors in log file
       checkErrors();
       
       return true;
    }
    return false;
}

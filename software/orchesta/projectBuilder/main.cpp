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
**	  main.cpp: Project builder main file
**
**	author: thom25@users.sourceforge.net
*/
#include <qcoreapplication>
#include <qstring>
#include <qstringlist>
#include <qfile>
//#include <qtextstream>
#include <iostream>
#include <qdir>
#include <qdatetime>
#include <qprocess>

#include "platform.h"
#include "project.h"
#include "ip.h"
#include "line_option.h"
#include "toolchain.h"
#include "armadeus_ready.h"


bool buildXilinx( Project &pro ){
    XilinxToolchain toolchain;
    if( pro.get(&toolchain)){
        if( !toolchain.buildXilinxPrj() )
            return false;
        if( toolchain.execute()) {
            return true;
        }
    }
    printf("Error: no valid toolchain\n");
    return false;
}


bool initProject(Project *pro){
    QDir dir;
    QStringList list = dir.entryList(QStringList("*.pro"));
    if( list.isEmpty() ){
        printf("Please create a local project first\n");
        return false;
    }
    pro->initialize(list.at(0));        
    return true;
}

void buildProject_callback(QStringList args){
    Project pro;

    if( initProject(&pro) ){
    	Platform plt;
        pro.computeIpBaseAddresses();
        if( pro.get(&plt) ) {
            if( plt.getFPGAFamily() == "spartan" ){
                buildXilinx(pro);
                pro.saveToFile();
            }
            else
                printf("unsupported FPGA Type\n");
        }
        else
            printf("Error no platform found \n");
    }
    else
        printf("Error no valid project\n");    
}


void addIp_callback( QStringList args ){
    Project pro;
    if( initProject(&pro) ){
        ArmadeusReadyIpFinder finder;
        finder.setPath(QCoreApplication::applicationDirPath()+ARMADEUS_READY_FOLDER);
        if( pro.addIp(args.at(0), finder.getIpAbsoluteFilePath(args.at(1))) )
            pro.saveToFile();
    }
};

void createNewProject_callback( QStringList args ){
    Project pro;

    if( pro.create(args.at(0)) ) {
        if( pro.addPlatform(args.at(1)) )
            pro.saveToFile();
    }
};

void removeIp_callback( QStringList args ){
    Project pro;
    if( initProject(&pro) ){
        if( pro.removeIp(args.at(0)) )
            pro.saveToFile();
    }
};

void listIps_callback(QStringList args){
    Project pro;
    if( initProject(&pro) ){
        printf("IP list:\n");
        QStringList ipNameList = pro.getIpNames();
        for( int i=0; i<ipNameList.count(); i++ )
            printf("%s\n",ipNameList.at(i).toLatin1().constData());
    }
};

void viewIpInfos_callback(QStringList args){
    Project pro;
    if( initProject(&pro) ){
        Ip ip;
        if( pro.get(args.at(0), &ip) )
            printf("%s",ip.toString().toLatin1().constData());    
    }
}

void viewPlatformInfos_callback(QStringList args){
    Project pro;
    if( initProject(&pro) ){
        Platform plt;
        if( pro.get(&plt) )
            printf("%s",plt.toString().toLatin1().constData());    
    }
}



void clearIpBaseAddress_callback(QStringList args){
    Project pro;
    if( initProject(&pro) ){
        Ip ip;
        if( pro.get(args.at(0), &ip) )
            ip.setBaseAddress("");    
    }
}

void setIpBaseAddress_callback(QStringList args){
    Project pro;
    if( initProject(&pro) ){
        Ip ip;
        if( pro.get(args.at(0), &ip) )
            ip.setBaseAddress(args.at(1));    
    }
}

void setIpInterrupt_callback(QStringList args){
    Project pro;
    if( initProject(&pro) ){
        Ip ip;
        if( pro.get(args.at(0), &ip) )
            ip.setInterrupt(args.at(1));    
    }
}

void listArmadeusReadyIps_callback(QStringList args){
    ArmadeusReadyIpFinder finder;
    finder.setPath(QCoreApplication::applicationDirPath()+ARMADEUS_READY_FOLDER);
    printf("%s",finder.getAllIpNames().join("\n").toLatin1().constData());
}

void setToolchain_callback(QStringList args){
    Project pro;

    if( initProject(&pro) ) {
        if( pro.setToolchain(args.at(0)) )
            pro.saveToFile();
    }
}


/*
Tools:
projetcBuilder: this program is used to generate the target firmware based on the specified IPs.

Project creation
Before to be able to add Ips to a project, it has to be created
projectBuilder -createProject test platform.xml 
Where test is the name of the project and platform.xml the platform description file. This file contains informations like the type of the FPGA used on a particular platform
 Result: creation of a  test.pro file.
 
 Adding/removing IPs
 
 once the project created, IPs can be added like that:
 projectBuilder -addIp myIp ip.xml ip_constraint.xml
 where myIp is the instance name of the IP ip.xml and ip_constraint.xml the constraint file fior myIp.
 The local .pro file is used that's why the it must not be specified in the command line

 Ips can be removed like that:
 projectBuilder -removeIp myIp
  
 Project informations
 
 List of Ips:
 The Ips present in the project can be listed as follow:
 projectBuilder -listIps
 The available Armadeus Ips can be listed as follow:
 projectBuilder -listArmadeusReadyIps
 These Ips can be easily used within a project by  means of the addIp command
 
 IP information:
 Informations concerning a particular Ip can be retrieved like that:
 projectBuilder -viewIpInfos myIp
 Platform informations can be displayed as follow:
 projectBuilder -viewPlatformInfos
  
IP set functions:
An IP instance has its own base address and its own interrupt ID within a project.  This two parameters can be changed like that:
projectBuilder -clearIpAddress myIP
projectBuilder -setIPBaseAddress myIP 0X0008   where 0X0008 is the new base address of the IP  
projectBuilder -setInterrupt myIP 0X0001 where 0X0001 si the new interrupt number of the IP

Toolchain functions:
The toolchain which will be used to compile the project can be specified by means of this command
The Xilinx toolchain is only supported even if the Altera one can be specified.
projectBuilded-setToolchain xilinx

Project Build
Once the IPs added to your project, it can be build with the following command:
 projectBuilder -buildProject
 results: several files are created:
 test.scr : script file for the Xilinx XST compiler
 test.prj: project file for the Xilinx XST compiler
 test.log log file

XST input files:
HDL, .prj, .scr
XST output file
NGO

NGDBuild input files:
.NGO
.ucf
NGDBuild output file:
NGD

MAP input files:
.NGD
MAP output files:
.NCD
NGM used for backannotation
MRP map report
.pcf optional. physical constraint file
MAP syntax
map [options] infile[.ngd] [pcf_file[.pcf]]
page 130 du manual

DRC
input file
NGD
output file
None

PAR input files:
NCD
PCF
PAR output files:
NCD
 CSV, PAD, TXT, GRF
 
 
 */




/* typical usage
* -createProject myProject apf9328
* -setToolchain xilinx
* -addIp firstUart uart.xml uartConstraint
* -buildProject
*/
 
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

	LineOption buildProject( "-buildProject", "-buildProject Build project",buildProject_callback);
	
	LineOption createProject( "-createProject", "-createProject [ProjectName] [Platform.xml] Create a new project using the given platform",createNewProject_callback);
	LineOption addIp( "-addIp", "-addIp [IPName] [IP.xml] [constraint.xml] Add a new IP based on IP.xml",addIp_callback);
	LineOption removeIp( "-removeIp", "-removeIp [IPName] Remove the IP",removeIp_callback);
	LineOption listIps( "-listIps", "-listIps List all IPs present in the local project",listIps_callback);
	LineOption viewIpInfos( "-viewIpInfos", "-viewIpInfos [IPName] display the IP informations", viewIpInfos_callback);
	LineOption viewPlatformInfos( "-viewPlatformInfos", "-viewPlatformInfos display the platform informations", viewPlatformInfos_callback);
	LineOption clearIpBaseAddress( "-clearIpBaseAddress", "-clearIpBaseAddress [IPName] set the base address of the given to undefined", clearIpBaseAddress_callback);
	LineOption setIpBaseAddress( "-setIpBaseAddress", "-setIpBaseAddress [IPName] [Address] set the base address in hex of the given IP", setIpBaseAddress_callback);
	LineOption setIpInterrupt( "-setIpInterrupt", "-setIpInterrupt [IPName] [Interrupt] set the interrupt number in hex of the given IP", setIpInterrupt_callback);
	LineOption listArmadeusReadyIps( "-listArmadeusReadyIps", "-listArmadeusReadyIps List all IPs present in the Armadeus Ready folder", listArmadeusReadyIps_callback);
	LineOption setToolchain( "-setToolchain", "-setToolchain [ToolchainName] Set the toolchain (xilinx/altera) used to build the project",setToolchain_callback);

   
	QStringList args = app.arguments();
	args.removeFirst();

    LineOptionManager optManager("Orchestra Version 0.1");
    optManager.add(&createProject);
    optManager.add(&addIp);    
    optManager.add(&removeIp);    
    optManager.add(&listIps);   
    optManager.add(&viewIpInfos); 
    optManager.add(&viewPlatformInfos); 
    optManager.add(&clearIpBaseAddress); 
    optManager.add(&setIpBaseAddress); 
    optManager.add(&setIpInterrupt);
    optManager.add(&buildProject);
    optManager.add(&listArmadeusReadyIps);
    optManager.add(&setToolchain);
    
    optManager.execute(args);
    
    
    
    return 0;
}


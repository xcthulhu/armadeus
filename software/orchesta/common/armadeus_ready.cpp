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
**	 armadeus_ready.cpp: armadeus_ready utilities
**
**	author: thom25@users.sourceforge.net
*/
#include "armadeus_ready.h"

#include <qdir>
#include <qfileinfo>
#include <qstringlist>

void ArmadeusReadyIpFinder::getNextIpAbsoluteNames(QFileInfoList *ipNamesList, QDir dir){
    QDir fileDir(dir.path()+"/");
    fileDir.setFilter(QDir::Files | QDir::NoSymLinks);
    fileDir.setNameFilters(QStringList("*.xml"));

    // start with local files
    QFileInfoList list = fileDir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        ipNamesList->append(list.at(i));
    }
    // continue with local folders
    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    list = dir.entryInfoList();
    for ( int i = 0; i < list.size(); ++i) {
        getNextIpAbsoluteNames(ipNamesList, QDir(list.at(i).absoluteFilePath()+"/") );
    }
}

QString ArmadeusReadyIpFinder::getIpAbsoluteFilePath( QString fileName ){
    QFileInfoList ipList = getAllIpAbsoluteNames();
    for( int i = 0; i<ipList.size(); i++ ){
        if( ipList.at(i).baseName() == fileName )
            return ipList.at(i).absoluteFilePath();
    }
    return QString();
}



void ArmadeusReadyIpFinder::setPath( QString path ){
    armadeusReadyPath = path;
}

QFileInfoList ArmadeusReadyIpFinder::getAllIpAbsoluteNames(){
    QFileInfoList result;
    QDir dir(armadeusReadyPath);
    
    dir.setFilter(QDir::AllDirs);
    getNextIpAbsoluteNames(&result, dir);
    
    return result;

}

QStringList ArmadeusReadyIpFinder::getAllIpNames(){
    QStringList result;
    QFileInfoList ipList = getAllIpAbsoluteNames();
    for( int i = 0; i<ipList.size(); i++ ){
        result.append(ipList.at(i).baseName());
    }
    
    return result;
}

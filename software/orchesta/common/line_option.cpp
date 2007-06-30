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
**	 line_option.cpp: command line option parser
**
**	author: thom25@users.sourceforge.net
*/
#include <QString>
#include <QStringList>

#include "line_option.h"

LineOption::LineOption( QString opt, QString help,  OptionCallback aCallback ) {
			option = opt; verbose = help; 
            nbArgs = help.count('['); 
            optionCallback = aCallback;
};

bool LineOption::process(QStringList list) {
    QStringList argsList;
    if( (!list.isEmpty()) && (list.at(0) == option) ) {
        list.removeFirst();
        if( list.size() == nbArgs )	{
             optionCallback(list);
             return true;
        }
    }
    return false;
};

QString LineOption::getHelp() {
    return verbose;
};

LineOptionManager::LineOptionManager(QString aMenu){
    menu = aMenu;
}

int LineOptionManager::getMaxCmdLength(){
    int maxCmdLength = 0;
    int pos = 0;
    for( int i=0; i<optionList.size(); i++ ){
        int tempMaxCmdLength = optionList.at(i)->getHelp().indexOf(' ');
        if( (pos != -1) && (tempMaxCmdLength>maxCmdLength) ){
            maxCmdLength = tempMaxCmdLength;
        }
    }
    return maxCmdLength;
}

void LineOptionManager::formatCmd(QString *string, int maxLength){
    int pos = string->indexOf(' ');
    if( pos < maxLength ){
        for( int i = pos; i<maxLength; i++)
            string->insert(pos,' '); 
    }
}

void LineOptionManager::displayHelp(){
    printf("%s\n",menu.toLatin1().constData());
    
    QStringList helpList;
    int maxCmdLength = getMaxCmdLength();
    
    for( int i=0; i<optionList.size(); i++ ){
        QString cmd = optionList.at(i)->getHelp();
        formatCmd(&cmd,maxCmdLength);    
        printf("%s\n", cmd.toLatin1().constData());
    }
}
    
void LineOptionManager::add(LineOption *opt){
    optionList.append( opt );
};

void LineOptionManager::execute(QStringList args){
    for( int i=0; i<optionList.size(); i++ ){
        if( optionList.at(i)->process(args) ){
            return;
        }
    }
    displayHelp();
}

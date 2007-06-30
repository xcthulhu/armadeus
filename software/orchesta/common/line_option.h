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
**	 line_option.h: command line option parser
**
**	author: thom25@users.sourceforge.net
*/
#ifndef LINE_OPTION_H
#define LINE_OPTION_H
#include <QString>
#include <QStringList>

typedef void (*OptionCallback)(QStringList);

class LineOption
{
	protected:
		QString option;
		QString verbose;
        OptionCallback optionCallback;
		int nbArgs;
		
	public:
		LineOption(){};
		LineOption( QString opt, QString help,  OptionCallback aCallback );
		~LineOption(){};
		
		bool process(QStringList list);
		QString getHelp();
};

class LineOptionManager
{
    protected:
        QString menu;
        QList<LineOption*> optionList;
    
        void displayHelp();
        int getMaxCmdLength();
        void formatCmd(QString *string, int maxLength);
     
    public:
        LineOptionManager(){};
        LineOptionManager(QString aMenu);
        ~LineOptionManager(){};
        
        void add(LineOption *opt);
        void execute(QStringList args);
};

#endif // LINE_OPTION_H

/*
**	THE ARMADEUS PROJECT
**
**  Copyright (2006)  The source forge armadeus project team
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
**	File name: ApfHostChooser.h
**
**	author: carbure@users.sourceforge.net
*/

#ifndef	__APFHOSTCHOOSER_H_INCLUDED__
#define	__APFHOSTCHOOSER_H_INCLUDED__

#include <QString>
#include "ui_ApfHostSelectDialog.h"

using namespace Ui;


class ApfHostChooser : public QDialog, public ApfHostSelectDialog
{

Q_OBJECT

public:

    ApfHostChooser(bool *cancelled, QString & SelectedHostName);

    virtual ~ApfHostChooser();
	void initialize();

public slots:

	void connectToServer();


protected:
	bool *mCancelled;
    QString  *mSelectedHostName;

};
#endif	// __APFHOSTCHOOSER_H_INCLUDED__


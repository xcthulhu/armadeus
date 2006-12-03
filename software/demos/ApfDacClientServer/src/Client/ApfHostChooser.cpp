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
**	File name: ApfHostChooser.cpp
**
**	author: carbure@users.sourceforge.net
*/

#include "ApfHostChooser.h"


//******************************************************************************

ApfHostChooser::ApfHostChooser(bool *cancelled, QString & SelectedHostName)
{
	mCancelled=cancelled;
	*mCancelled=true;
    mSelectedHostName = &SelectedHostName;
}

//******************************************************************************

ApfHostChooser::~ApfHostChooser()
{
}

//******************************************************************************
void ApfHostChooser::initialize()
{
	setupUi(this);
	connect( cancelButton, SIGNAL(clicked()),this, SLOT(close()) );
    connect( okButton, SIGNAL(clicked()),this, SLOT(connectToServer()) );
}

//******************************************************************************
void ApfHostChooser::connectToServer()
{
	*mCancelled=false;
	*mSelectedHostName = hostNameComboBox->currentText();
	close();
}

//******************************************************************************


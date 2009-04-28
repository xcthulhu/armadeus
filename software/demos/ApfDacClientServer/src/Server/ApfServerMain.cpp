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
**	 ApfServerMain.cpp: main for ATS server
**
**	author: carbure@users.sourceforge.net
*/


#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#ifndef NOSERVERGUI
#include <QApplication>
# ifdef Q_WS_QWS
#include <QWSServer>
# endif
#else
#include <QCoreApplication>
#endif

#include <iostream>

#include "ApfServer.h"

//using namespace std;

void runAsDaemon()
{
    int    pid( fork());

    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
    we can exit the parent process. */
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
    sleep(2);
    umask(0);
    int sid (setsid());
    if (sid < 0)
    {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0)
    {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}


//******************************************************************************

int main(int argc, char **argv)
{
#ifndef QT_DEBUG
 //   runAsDaemon( );
#endif

#ifndef NOSERVERGUI
    QApplication a( argc, argv );
# ifdef Q_WS_QWS
    // Hide mouse cursor:
    QWSServer *sw =  QWSServer::instance ();
    sw->setCursorVisible(false);
# endif
    // Show UI
    ApfServer AtsServer;
    AtsServer.show();
    return a.exec();
#else
    QCoreApplication a( argc, argv );
    ApfServer AtsServer;
    return a.exec();
#endif
}

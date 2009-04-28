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
**	 ApfServer.h
**
**	author: carbure@users.sourceforge.net
*/


#ifndef	__APFSERVER_H_INCLUDED__
#define	__APFSERVER_H_INCLUDED__

#include <QTcpSocket>
#include <QTcpServer>


#include <qstring.h>


#include "ApfNetworkDefs.h"
#include <QObject>

#ifndef NOSERVERGUI
#include <QDialog>
#include <qpushbutton.h>
#include "ui_ApfServerTabDialog.h"
#include "../widgets/qled.h"
#include "../widgets/manometer.h"
#include "../widgets/thermometer.h"
#include "../widgets/qslide.h"
#endif

class ApfServerListener;

using namespace ApfNetworkDefs;

#ifndef NOSERVERGUI
class ApfServer : public QDialog, public Ui::ApfServerTabDialog
#else
class ApfServer : public QObject
#endif
{
    Q_OBJECT

public:

    ApfServer( );
    virtual ~ApfServer();
    void PutServerDebugMessage(const char * str);
    static ApfServer *getApfServerInstance();

public slots:

    void PutServerDebugMessage(const QString & str);
    void EndServer();
    void setLedValue(int id, bool);
    void setManometerValue(int);
    void setThermometerValue(int);
    void setSlideText(const QString &);

private:

    ApfServerListener *mApfServerListener;
#ifndef NOSERVERGUI
    static const int MAX_TEXT_EDIT_LENGTH = 100000;
#endif

protected:

    static ApfServer   * gApfServer;
#ifndef NOSERVERGUI
    QLed* mRedLed;
    QLed* mGreenLed;
    ManoMeter* mManometer;
    ThermoMeter* mThermometer;
    QGridLayout* mWidgetsLayout;
    QSlide* mSlide;
#endif
};

#endif	// __APFSERVER_H_INCLUDED__

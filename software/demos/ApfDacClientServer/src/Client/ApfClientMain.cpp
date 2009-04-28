//
//   ApfClientMain.cpp
//
//   Copyright  : (c) 2007 Armadeus Project
//
//

#include <iostream>
#include <qapplication.h>

#include "ApfHostChooser.h"
#include "ApfClient.h"

int main(int argc, char **argv)
{    
    QApplication a( argc, argv );
    QString hostUnderTest("");
    QStringList oldSelections;
    bool   cancelled;

    ApfHostChooser theApfHostChooser( &cancelled,hostUnderTest );
	
    theApfHostChooser.initialize();
    theApfHostChooser.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );    
    a.exec();
    if (cancelled == true)
    {
        qDebug("User has cancelled");
        return 1;
    } else {
        ApfClient theApfClient(hostUnderTest);

        theApfClient.run();
        qDebug("Selected host: %s",hostUnderTest.toAscii().data());
        a.exec();  
        return 0;		
    }
}


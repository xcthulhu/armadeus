#
#      THE ARMADEUS PROJECT
#
#  Copyright (2006)  The source forge armadeus project team
#

This is a TCP Client/server demo program to control the DAC of an Apf9828 board over a network. 
The build script will generate three Client programs , three Server with GUI and  three Server without 
GUI for the following targets:

* Linux embedded Qtopia 4.2.0 core (./bin/apf)
* Standart linux PC with a QT4.2.0 installation (./bin/linux)
* An additional version for the linux PC with QT 4.2.0 virtual frame buffer. (./bin/qvbf)

You should have installed theses three versions of QT 4 according to the documentation :
http://www.armadeus.com/wiki/index.php?title=Qt/Embedded

To build the different components, type:

> cd src
> sh build.sh

To clean up:

> sh build clean



The server is able to works without I2C bus. But you should ativate it as described in the DAC documentation page:
http://www.armadeus.com/wiki/index.php?title=DAC
If running on a PC, the server will only simulate the DAC programming.

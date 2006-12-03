
SERVER_DIR= .
NETWORKING_DIR= ../Networking

CPP_ALWAYS_CREATE_SOURCE = TRUE

TEMPLATE	=app
QT += network
QT -= gui

CONFIG	+= qt thread warn_on release

DEFINES += NOSERVERGUI

LANGUAGE	= C++

DEPENDPATH += $$NETWORKING_DIR/ \
	$$SERVER_DIR

HEADERS =  \
	$$SERVER_DIR/ApfDacController.h \
	$$NETWORKING_DIR/ApfNetworkDefs.h \
	$$SERVER_DIR/ApfServerListener.h \
	$$SERVER_DIR/ApfServer.h \
	$$SERVER_DIR/ApfCore.h

SOURCES = \
 	$$SERVER_DIR/ApfDacController.cpp \
	$$SERVER_DIR/ApfServer.cpp \
	$$SERVER_DIR/ApfCore.cpp \
 	$$SERVER_DIR/ApfServerListener.cpp \
	$$SERVER_DIR/ApfServerMain.cpp

INCLUDEPATH += \
	$$DEPENDPATH 

TARGET = ApfServerNoGui

OBJECTS_DIR = $(OUTPUT_DIR)/noGuiobjs/
MOC_DIR = $(OUTPUT_DIR)/noGuimocs/
UI_DIR= $(OUTPUT_DIR)//noGuiuis

LIBS += \
        -L$$NETWORKING_DIR/$(OUTPUT_DIR) -lApfNetwork

        
DESTDIR = $(OUTPUT_DIR)

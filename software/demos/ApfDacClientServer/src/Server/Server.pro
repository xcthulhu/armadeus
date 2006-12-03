
SERVER_DIR= .
NETWORKING_DIR= ../Networking

CPP_ALWAYS_CREATE_SOURCE = TRUE

TEMPLATE = app
CONFIG	+= qt thread warn_on release
QT += network

LANGUAGE = C++

FORMS = $$SERVER_DIR/ApfServerDialog.ui

DEPENDPATH += $$NETWORKING_DIR/ \
	$$SERVER_DIR		

HEADERS =      \
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

TARGET = ApfServer

OBJECTS_DIR = $(OUTPUT_DIR)/objs/
MOC_DIR = $(OUTPUT_DIR)/mocs/
UI_DIR = $(OUTPUT_DIR)/uis

LIBS += \
        -L$$NETWORKING_DIR/$(OUTPUT_DIR) -lApfNetwork
        
DESTDIR = $(OUTPUT_DIR)


CLIENT_DIR= ./
NETWORKING_DIR= ../Networking
QT +=network

CPP_ALWAYS_CREATE_SOURCE = TRUE

TEMPLATE = app
CONFIG	+= qt thread warn_on 
LANGUAGE = C++

FORMS = $$CLIENT_DIR/ApfHostSelectDialog.ui \
	$$CLIENT_DIR/Vmeters.ui

HEADERS = \
    	$$NETWORKING_DIR/ApfNetworkDefs.h \
    	$$NETWORKING_DIR/ApfNetworkOperation.h \
    	$$NETWORKING_DIR/ApfNetworkProtocol.h \
	$$CLIENT_DIR/ApfClient.h \
	$$CLIENT_DIR/ApfHostChooser.h

SOURCES = $$NETWORKING_DIR/ApfNetworkOperation.cpp \
    	$$NETWORKING_DIR/ApfNetworkProtocol.cpp \
	$$CLIENT_DIR/ApfHostChooser.cpp \
	$$CLIENT_DIR/ApfClient.cpp \
    	$$CLIENT_DIR/ApfClientMain.cpp
    
DEPENDPATH += \
    $$NETWORKING_DIR \
    $$CLIENT_DIR 

INCLUDEPATH += \
    $$DEPENDPATH 

TARGET = ApfClient

OBJECTS_DIR = $(OUTPUT_DIR)/objs/
MOC_DIR = $(OUTPUT_DIR)/mocs/
UI_DIR= $(OUTPUT_DIR)/uis

DESTDIR = $(OUTPUT_DIR)



NETWORKING_DIR= ./

QT -=gui
CONFIG += warn_on qt thread release

TEMPLATE = lib

DEPENDPATH += \
    $$NETWORKING_DIR

HEADERS = \
    $$NETWORKING_DIR/ApfNetworkDefs.h \
    $$NETWORKING_DIR/ApfNetworkOperation.h \
    $$NETWORKING_DIR/ApfNetworkProtocol.h

SOURCES = \
    $$NETWORKING_DIR/ApfNetworkOperation.cpp \
    $$NETWORKING_DIR/ApfNetworkProtocol.cpp

INCLUDEPATH += \
    $$DEPENDPATH 

TARGET = ApfNetwork

OBJECTS_DIR = $(OUTPUT_DIR)/objs/
MOC_DIR = $(OUTPUT_DIR)/mocs/

DESTDIR = $(OUTPUT_DIR)



WIDGET_DIR = ./

CONFIG += warn_on qt thread release

TEMPLATE = lib

DEPENDPATH += \
    $$WIDGET_DIR

HEADERS = \
    $$WIDGET_DIR/manometer.h \
    $$WIDGET_DIR/widgetwithbackground.h \
    $$WIDGET_DIR/abstractmeter.h \
    $$WIDGET_DIR/thermometer.h \
    $$WIDGET_DIR/functions.h \
    $$WIDGET_DIR/qled.h \
    $$WIDGET_DIR/qslide.h

SOURCES = \
    $$WIDGET_DIR/manometer.cpp \
    $$WIDGET_DIR/widgetwithbackground.cpp \
    $$WIDGET_DIR/abstractmeter.cpp \
    $$WIDGET_DIR/thermometer.cpp \
    $$WIDGET_DIR/qled.cpp \
    $$WIDGET_DIR/qslide.cpp

INCLUDEPATH += \
    $$DEPENDPATH

TARGET = ApfWidget

OBJECTS_DIR = $(OUTPUT_DIR)/objs/
MOC_DIR = $(OUTPUT_DIR)/mocs/

DESTDIR = $(OUTPUT_DIR)


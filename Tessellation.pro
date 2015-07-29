HOME_LOCAL = /home/broy/dev/local

QMAKE_CXXFLAGS += -std=gnu++0x
QT += core gui opengl xml
TARGET = Tessellation
TEMPLATE = app

HEADERS += include/*.h
SOURCES += src/*.cpp
FORMS += gui/mainWindow.ui
OTHER_FILES += shaders/*.vs shaders/*.fs

INCLUDEPATH += include $$HOME_LOCAL/include
LIBS += -L/usr/lib/ -lGL -lGLEW
LIBS += -L$$HOME_LOCAL/lib -lQGLViewer

DESTDIR = .
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

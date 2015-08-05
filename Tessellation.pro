QMAKE_CXXFLAGS += -std=gnu++0x
QT += core gui opengl xml
TARGET = Tessellation
TEMPLATE = app

HEADERS += include/*.h
SOURCES += src/*.cpp
FORMS += gui/mainWindow.ui
OTHER_FILES += shaders/render.*

INCLUDEPATH += include
LIBS += -L/usr/lib/x86_64-linux-gnu -lGL -lGLU -lGLEW
LIBS += -lQGLViewer

DESTDIR = .
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

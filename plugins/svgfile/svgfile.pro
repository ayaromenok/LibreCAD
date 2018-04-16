
QT       += gui
TEMPLATE = lib
CONFIG += plugin
VERSION = 1.0.0
TARGET = $$qtLibraryTarget(svgfile)

GENERATED_DIR = ../../generated/plugin/svgfile
# Use common project definitions.
include(../../common.pri)

# For plugins
INCLUDEPATH    += ../../librecad/src/plugins

SOURCES += svgfile.cpp
HEADERS += svgfile.h

# Installation Directory
win32 {
        DESTDIR = ../../windows/resources/plugins
}
unix {
    macx {
        DESTDIR = ../../LibreCAD.app/Contents/Resources/plugins
    }
    else {
        DESTDIR = ../../unix/resources/plugins
    }
}

######################################################################
# Automatically generated by qmake (3.1) Mon Aug 2 15:02:01 2021
######################################################################

TEMPLATE = app
TARGET = hexchess
INCLUDEPATH += .

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QT += gui svg widgets
QMAKE_CXX = clang++
QMAKE_CXXFLAGS += -std=c++17 -g -fPIC
INCLUDEPATH += core ui
OBJECTS_DIR = obj
DESTDIR = obj

HEADERS += \
    core/board.h core/fen.h core/geometry.h core/move.h core/variant.h core/zobrist.h \
    \
    ui/mainWindow.h ui/boardwidget.h \
    ui/stylecolor.h ui/stylefont.h ui/styleicon.h ui/stylemeasure.h \
    ui/util_ui.h

SOURCES += main.cpp \
    \
    core/board.cpp \
    core/fen.cpp \
    core/geometry.cpp \
    core/move.cpp \
    core/util_hexchess.cpp \
    core/variant.cpp \
    core/zobrist.cpp \
    core/zobrist_table.cpp \
    \
    ui/boardwidget.cpp \
    ui/stylecolor.cpp ui/styleicon.cpp ui/stylemeasure.cpp \
    ui/util_ui.cpp

TEMPLATE = app
TARGET = test
INCLUDEPATH += ../src ../src/core ../src/player ../src/server ../src/ui
VPATH += ../src
DEFINES += QT_DEPRECATED_WARNINGS

# disables all the APIs deprecated before Qt 6.0.0
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

QT += gui svg testlib widgets
QMAKE_CXX = clang++
QMAKE_CXXFLAGS += -std=c++2a -g -fPIC
QMAKE_CXXFLAGS_WARN_ON = -Werror \
    -Wno-error=unused-parameter -Wno-error=unknown-escape-sequence
QMAKE_LIBS += -lgtest

MOC_DIR = obj
OBJECTS_DIR = obj
DESTDIR = obj

CORE = ../src/core
PLAYER = ../src/player
SERVER = ../src/server
UI = ../src/ui
TEST = .

HEADERS += \
    $$CORE/board.h $$CORE/fen.h $$CORE/game_outcome.h \
    $$CORE/geometry.h $$CORE/move.h $$CORE/variant.h \
    $$CORE/zobrist.h \
    \
    $$PLAYER/player.h \
    $$PLAYER/player_preference.h \
    $$PLAYER/player_random.h \
    \
    $$SERVER/server.h \
    \
    $$UI/mainwindow.h $$UI/boardwidget.h $$UI/stylecolor.h \
    $$UI/stylefont.h $$UI/styleicon.h $$UI/stylemeasure.h \
    $$UI/util_ui.h

SOURCES += $$TEST/test.cpp \
    $$TEST/test_board.cpp $$TEST/test_fen.cpp $$TEST/test_game.cpp \
    $$TEST/test_geometry.cpp $$TEST/test_move.cpp $$TEST/test_player.cpp \
    $$TEST/test_zobrist.cpp \
    \
    $$CORE/board.cpp $$CORE/fen.cpp $$CORE/game_outcome.cpp \
    $$CORE/geometry.cpp $$CORE/move.cpp $$CORE/player_action.cpp \
    $$CORE/util_hexchess.cpp $$CORE/variant.cpp $$CORE/zobrist.cpp \
    $$CORE/zobrist_table.cpp \
    \
    $$PLAYER/player_preference.cpp \
    $$PLAYER/player_random.cpp \
    \
    $$SERVER/server.cpp \
    \
    $$UI/boardwidget.cpp $$UI/stylecolor.cpp $$UI/styleicon.cpp \
    $$UI/stylemeasure.cpp $$UI/util_ui.cpp

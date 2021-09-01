# Copyright (C) 2021, by Jay M. Coskey
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

MAKEFLAGS=--warn-undefined-variables

CXXFLAGS =
LDFLAGS =
TARGET_ARCH =
LOADLIBES =
LDLIBS =

SRC_BASE_DIR=../src
SRC_CORE_DIR=$(SRC_BASE_DIR)/core
TEST_DIR=.
OBJ_DIR=obj

CC=clang++
# CC=g++
CPPFLAGS=-std=c++17 -Wall -g -I$(SRC_BASE_DIR) -I$(SRC_CORE_DIR) -I$(TEST_DIR)
# CPPFLAGS += -Werror, to treat all warnings as errors
# CPPFLAGS += -Wno-error=X, to override -Werror for any specified condition X.

.PHONY: build test

default: test

# APP=hexchess
TESTS= \
	$(OBJ_DIR)/test_geometry \
	$(OBJ_DIR)/test_board    \
	$(OBJ_DIR)/test_fen      \
	$(OBJ_DIR)/test_zobrist  \
	$(OBJ_DIR)/test_move     \
	$(OBJ_DIR)/test_player   \
	$(OBJ_DIR)/test_game

clean:
	rm -f $(OBJ_DIR)/*.o $(TESTS)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

build: $(TESTS)
	echo "========== Tests ready to be run =========="

# Execution of tests proceeds from simpler to more complex functionality.
test: build
	$(foreach t, $(TESTS), ./$(t))

$(OBJ_DIR)/%.o: $(SRC_BASE_DIR)/%.cpp $(OBJ_DIR)
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_CORE_DIR)/%.cpp $(OBJ_DIR)
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp $(OBJ_DIR)
	$(CC) $(CPPFLAGS) -c -o $@ $<

TEST_BOARD_H = board.h fen.h geometry.h move.h util_hexchess.h zobrist.h
TEST_BOARD_CPP = board.cpp fen.cpp geometry.cpp move.cpp \
                    util_hexchess.cpp variant.cpp zobrist.cpp zobrist_table.cpp

TEST_FEN_H = fen.h geometry.h util_hexchess.h
TEST_FEN_CPP = fen.cpp geometry.cpp util_hexchess.cpp variant.cpp

TEST_GAME_H = board.h fen.h game.h game_outcome.h geometry.h move.h \
              player.h util_hexchess.h zobrist.h
TEST_GAME_CPP = board.cpp fen.cpp game.cpp game_outcome.cpp geometry.cpp move.cpp \
                player.cpp util_hexchess.cpp variant.cpp zobrist.cpp zobrist_table.cpp

TEST_GEOMETRY_H = fen.h geometry.h util_hexchess.h variant.h
TEST_GEOMETRY_CPP = fen.cpp geometry.cpp util_hexchess.cpp variant.cpp

TEST_MOVE_H = board.h fen.h geometry.h move.h util_hexchess.h variant.h zobrist.h
TEST_MOVE_CPP = board.cpp fen.cpp geometry.cpp move.cpp \
                util_hexchess.cpp variant.cpp zobrist.cpp zobrist_table.cpp

TEST_PLAYER_H = board.h fen.h geometry.h move.h \
                player.h util_hexchess.h variant.h zobrist.h
TEST_PLAYER_CPP = board.cpp fen.cpp geometry.cpp move.cpp \
                  player.cpp util_hexchess.cpp variant.cpp zobrist.cpp zobrist_table.cpp

TEST_ZOBRIST_H = fen.h geometry.h util_hexchess.h variant.h zobrist.h
TEST_ZOBRIST_CPP = fen.cpp geometry.cpp \
                   util_hexchess.cpp variant.cpp zobrist.cpp zobrist_table.cpp


TEST_BOARD_HDRS    = $(TEST_BOARD_H:%.h=$(SRC_CORE_DIR)/%.h) $(SRC_BASE_DIR)/util.h
TEST_FEN_HDRS      = $(TEST_FEN_H:%.h=$(SRC_CORE_DIR)/%.h)
TEST_GAME_HDRS     = $(TEST_GAME_H:%.h=$(SRC_CORE_DIR)/%.h) $(SRC_BASE_DIR)/util.h
TEST_GEOMETRY_HDRS = $(TEST_GEOMETRY_H:%.h=$(SRC_CORE_DIR)/%.h) $(SRC_BASE_DIR)/util.h
TEST_MOVE_HDRS     = $(TEST_MOVE_H:%.h=$(SRC_CORE_DIR)/%.h) $(SRC_BASE_DIR)/util.h
TEST_PLAYER_HDRS   = $(TEST_PLAYER_H:%.h=$(SRC_CORE_DIR)/%.h) $(SRC_BASE_DIR)/util.h
TEST_ZOBRIST_HDRS  = $(TEST_ZOBRIST_H:%.h=$(SRC_CORE_DIR)/%.h) $(SRC_BASE_DIR)/util.h

TEST_BOARD_OBJS    = $(TEST_BOARD_CPP:%.cpp=$(OBJ_DIR)/%.o)
TEST_FEN_OBJS      = $(TEST_FEN_CPP:%.cpp=$(OBJ_DIR)/%.o)
TEST_GAME_OBJS     = $(TEST_GAME_CPP:%.cpp=$(OBJ_DIR)/%.o)
TEST_GEOMETRY_OBJS = $(TEST_GEOMETRY_CPP:%.cpp=$(OBJ_DIR)/%.o)
TEST_MOVE_OBJS     = $(TEST_MOVE_CPP:%.cpp=$(OBJ_DIR)/%.o)
TEST_PLAYER_OBJS   = $(TEST_PLAYER_CPP:%.cpp=$(OBJ_DIR)/%.o)
TEST_ZOBRIST_OBJS  = $(TEST_ZOBRIST_CPP:%.cpp=$(OBJ_DIR)/%.o)


$(OBJ_DIR)/test_board: $(TEST_BOARD_HDRS) $(TEST_BOARD_OBJS) $(OBJ_DIR)/test_board.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_board $(OBJ_DIR)/test_board.o $(TEST_BOARD_OBJS)

$(OBJ_DIR)/test_fen: $(TEST_FEN_HDRS) $(TEST_FEN_OBJS) $(OBJ_DIR)/test_fen.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_fen $(OBJ_DIR)/test_fen.o $(TEST_FEN_OBJS)

$(OBJ_DIR)/test_game: $(TEST_GAME_HDRS) $(TEST_GAME_OBJS) $(OBJ_DIR)/test_game.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_game $(OBJ_DIR)/test_game.o $(TEST_GAME_OBJS)

$(OBJ_DIR)/test_geometry: $(TEST_GEOMETRY_HDRS) $(TEST_GEOMETRY_OBJS) $(OBJ_DIR)/test_geometry.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_geometry $(OBJ_DIR)/test_geometry.o $(TEST_GEOMETRY_OBJS)

$(OBJ_DIR)/test_move: $(TEST_MOVE_HDRS) $(TEST_MOVE_OBJS) $(OBJ_DIR)/test_move.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_move $(OBJ_DIR)/test_move.o $(TEST_MOVE_OBJS)

$(OBJ_DIR)/test_player: $(TEST_PLAYER_HDRS) $(TEST_PLAYER_OBJS) $(OBJ_DIR)/test_player.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_player $(OBJ_DIR)/test_player.o $(TEST_PLAYER_OBJS)

$(OBJ_DIR)/test_zobrist: $(TEST_ZOBRIST_HDRS) $(TEST_ZOBRIST_OBJS) $(OBJ_DIR)/test_zobrist.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_zobrist $(OBJ_DIR)/test_zobrist.o $(TEST_ZOBRIST_OBJS)

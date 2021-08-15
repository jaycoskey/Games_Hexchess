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

SRC_TEST_DIR=.
SRC_CORE_DIR=../core
OBJ_DIR=.

CC=clang++
CPPFLAGS=-std=c++17 -g -I$(SRC_CORE_DIR) -I$(SRC_TEST_DIR)

.PHONY: test

default: test

# APP=hexchess
TESTS= \
	$(OBJ_DIR)/test_bitboard \
	$(OBJ_DIR)/test_geometry \
	$(OBJ_DIR)/test_move \
	$(OBJ_DIR)/test_piece \
	$(OBJ_DIR)/test_zobrist

clean:
	rm -f *.o $(TESTS)

test: $(TESTS)
	echo "Compilation done: test_bitboard, test_geometry, test_move, test_piece, test_zobrist"
	$(OBJ_DIR)/test_bitboard
	$(OBJ_DIR)/test_geometry
	$(OBJ_DIR)/test_move
	$(OBJ_DIR)/test_piece
	$(OBJ_DIR)/test_zobrist


$(OBJ_DIR)/%.o: $(SRC_CORE_DIR)/%.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<


TEST_BITBOARD_H = bitboard.h geometry.h util.h util_hexchess.h zobrist.h
TEST_BITBOARD_CPP = bitboard.cpp geometry.cpp util_hexchess.cpp variant.cpp zobrist.cpp zobrist_table.cpp

TEST_GEOMETRY_H = bitboard.h geometry.h util.h util_hexchess.h variant.h
TEST_GEOMETRY_CPP = geometry.cpp util_hexchess.cpp variant.cpp

TEST_MOVE_H = geometry.h move.h piece.h util.h util_hexchess.h variant.h
TEST_MOVE_CPP = geometry.cpp move.cpp piece.cpp util_hexchess.cpp variant.cpp

TEST_PIECE_H = geometry.h piece.h util.h util_hexchess.h variant.h
TEST_PIECE_CPP = geometry.cpp move.cpp piece.cpp util_hexchess.cpp variant.cpp

TEST_ZOBRIST_H = geometry.h util.h util_hexchess.h variant.h zobrist.h
TEST_ZOBRIST_CPP = geometry.cpp util_hexchess.cpp variant.cpp zobrist.cpp zobrist_table.cpp


TEST_BITBOARD_HDRS = $(TEST_BITBOARD_H:%.h=$(SRC_CORE_DIR)/%.h)
TEST_GEOMETRY_HDRS = $(TEST_GEOMETRY_H:%.h=$(SRC_CORE_DIR)/%.h)
TEST_MOVE_HDRS = $(TEST_MOVE_H:%.h=$(SRC_CORE_DIR)/%.h)
TEST_PIECE_HDRS = $(TEST_PIECE_H:%.h=$(SRC_CORE_DIR)/%.h)
TEST_ZOBRIST_HDRS = $(TEST_ZOBRIST_H:%.h=$(SRC_CORE_DIR)/%.h)

TEST_BITBOARD_SRCS = $(TEST_BITBOARD_CPP:%.cpp=$(SRC_CORE_DIR)/%.o)
TEST_GEOMETRY_SRCS = $(TEST_GEOMETRY_CPP:%.cpp=$(SRC_CORE_DIR)/%.o)
TEST_MOVE_SRCS = $(TEST_MOVE_CPP:%.cpp=$(SRC_CORE_DIR)/%.o)
TEST_PIECE_SRCS = $(TEST_PIECE_CPP:%.cpp=$(SRC_CORE_DIR)/%.o)
TEST_ZOBRIST_SRCS = $(TEST_ZOBRIST_CPP:%.cpp=$(SRC_CORE_DIR)/%.o)

TEST_BITBOARD_OBJS = $(TEST_BITBOARD_CPP:%.cpp=%.o)
TEST_GEOMETRY_OBJS = $(TEST_GEOMETRY_CPP:%.cpp=%.o)
TEST_MOVE_OBJS = $(TEST_MOVE_CPP:%.cpp=%.o)
TEST_PIECE_OBJS = $(TEST_PIECE_CPP:%.cpp=%.o)
TEST_ZOBRIST_OBJS = $(TEST_ZOBRIST_CPP:%.cpp=%.o)


test_bitboard: $(TEST_BITBOARD_HDRS) $(TEST_BITBOARD_OBJS) test_bitboard.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_bitboard test_bitboard.o $(TEST_BITBOARD_OBJS)

test_geometry: $(TEST_GEOMETRY_HDRS) $(TEST_GEOMETRY_OBJS) test_geometry.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_geometry test_geometry.o $(TEST_GEOMETRY_OBJS)

test_move: $(TEST_MOVE_HDRS) $(TEST_MOVE_OBJS) test_move.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_move test_move.o $(TEST_MOVE_OBJS)

test_piece: $(TEST_PIECE_HDRS) $(TEST_PIECE_OBJS) test_piece.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_piece test_piece.o $(TEST_PIECE_OBJS)

test_zobrist: $(TEST_ZOBRIST_HDRS) $(TEST_ZOBRIST_OBJS) test_zobrist.o
	$(CC) $(CPPFLAGS) -o $(OBJ_DIR)/test_zobrist test_zobrist.o $(TEST_BITBOARD_OBJS)

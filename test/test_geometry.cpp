// Copyright (C) 2021, by Jay M. Coskey
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <cassert>
#include <cstring>

#include <iostream>

#include "geometry.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"

using std::cout;

using hexchess::NotImplementedException;

using hexchess::core::Glinski;
using hexchess::core::HexDir;
using hexchess::core::HexPos;
using hexchess::core::Index;


void print_cell_names() {
    cout << "Cell names: ";
    for (Index i = 0; i < Glinski::CELL_COUNT; ++i) {
        cout << i << ": " << Glinski::cellName(i) << ", ";
    }
    cout << "\n";
}

void print_indexToHex0() {
    cout << "Index to hex0: ";
    for (Index i = 0; i < Glinski::CELL_COUNT; ++i) {
        cout << i << ": " << std::to_string(Glinski::hex0(i)) << ", ";
    }
    cout << "\n";
}

void print_indexToHex1() {
    cout << "Index to hex0: ";
    for (Index k = 0; k < Glinski::CELL_COUNT; ++k) {
        cout << k << ": " << std::to_string(Glinski::hex1(k)) << ", ";
    }
    cout << "\n";
}

/// \brief Test: Hex coordinate functions and addition of HexPos + HexDir work as expected.
void test_position_direction(bool verbose=false) {
    typedef Glinski V;

    const Index CENTER_INDEX = 45;
    HexPos knightHexPos{V::hex0(CENTER_INDEX), V::hex1(CENTER_INDEX)};
    assert(knightHexPos.hex0 == 5);
    assert(knightHexPos.hex1 == 5);

    if (verbose) {
        cout << "Center cell index = " << std::to_string(V::posToIndex(knightHexPos)) << "\n";
        cout << "Center cell name  = " << V::cellName(V::posToIndex(knightHexPos)) << "\n";
    }

    assert(V::cellName(V::posToIndex(knightHexPos)) == "F6");

    HexDir knightLeap{1, 2};
    HexPos knightDest = knightHexPos + knightLeap;
    assert(knightDest.hex0 == 6);
    assert(knightDest.hex1 == 7);
    assert(V::posToIndex(knightDest) == 65);
}

/// \brief Test: Test the count of all Queen moves over all starting points.
void test_move_count_king(bool verbose) {
    throw NotImplementedException{"test_geometry.cpp: test_move_count_king"};
}

/// \brief Test: Test the count of all Queen moves over all starting points.
void test_move_count_queen(bool verbose) {
    throw NotImplementedException{"test_geometry.cpp: test_move_count_queen"};
}

/// \brief Test: Test the count of all Rook moves over all starting points.
void test_move_count_rook(bool verbose) {
    throw NotImplementedException{"test_geometry.cpp: test_move_count_rook"};
}

/// \brief Test: Test the count of all Rook moves over all starting points.
void test_move_count_bishop(bool verbose) {
    throw NotImplementedException{"test_geometry.cpp: test_move_count_bishop"};
}

/// \brief Test: Test the count of all Knight moves over all starting points.
void test_move_count_knight(bool verbose) {
    throw NotImplementedException{"test_geometry.cpp: test_move_count_knight"};
}

int main(int argc, char *argv[]) {
    test_position_direction();
    // print_cell_names();
    // print_indexToHex0();
    // print_indexToHex1();
}

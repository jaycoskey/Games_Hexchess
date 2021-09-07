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

#include <cstring>

#include <iostream>

#include <gtest/gtest.h>

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


// Note: Currently not invoked
void print_cell_names() {
    cout << "Cell names: ";
    for (Index i = 0; i < Glinski::CELL_COUNT; ++i) {
        cout << i << ": " << Glinski::cellName(i) << ", ";
    }
    cout << "\n";
}

// Note: Currently not invoked
void print_indexToHex0() {
    cout << "Index to hex0: ";
    for (Index i = 0; i < Glinski::CELL_COUNT; ++i) {
        cout << i << ": " << std::to_string(Glinski::hex0(i)) << ", ";
    }
    cout << "\n";
}

// Note: Currently not invoked
void print_indexToHex1() {
    cout << "Index to hex0: ";
    for (Index k = 0; k < Glinski::CELL_COUNT; ++k) {
        cout << k << ": " << std::to_string(Glinski::hex1(k)) << ", ";
    }
    cout << "\n";
}

/// \brief Test: Hex coordinate functions and addition of HexPos + HexDir work as expected.
TEST(GeometryTest, GeometryPositiveDirection) {
    bool verbose = false;

    typedef Glinski V;

    const Index CENTER_INDEX = 45;
    HexPos knightHexPos{V::hex0(CENTER_INDEX), V::hex1(CENTER_INDEX)};
    ASSERT_EQ(knightHexPos.hex0, 5);
    ASSERT_EQ(knightHexPos.hex1, 5);

    if (verbose) {
        cout << "Center cell index = " << std::to_string(V::posToIndex(knightHexPos)) << "\n";
        cout << "Center cell name  = " << V::cellName(V::posToIndex(knightHexPos)) << "\n";
    }

    ASSERT_EQ(V::cellName(V::posToIndex(knightHexPos)), "F6");

    HexDir knightLeap{1, 2};
    HexPos knightDest = knightHexPos + knightLeap;
    ASSERT_EQ(knightDest.hex0, 6);
    ASSERT_EQ(knightDest.hex1, 7);
    ASSERT_EQ(V::posToIndex(knightDest), 65);
}

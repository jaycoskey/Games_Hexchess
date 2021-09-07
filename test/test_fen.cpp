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

#include <iostream>

#include <gtest/gtest.h>

#include "fen.h"
#include "util_hexchess.h"

using std::cout;
using std::string;

using hexchess::core::BoardDir;
using hexchess::core::Fen;
using hexchess::core::Glinski;


TEST(FenTest, FenBasic) {
    bool verbose = false;

    string fenInitial = Glinski::fenInitial;
    if (verbose) { cout << "FEN initial: " << fenInitial << "\n"; }
    Fen<Glinski> fen{fenInitial};
    ASSERT_EQ(fen.piecesSparse.size(), Glinski::CELL_COUNT);
    string fenStr = fen.fen_string();
    if (verbose) { cout << "FEN derived: " << fenStr << "\n"; }
    ASSERT_EQ(fenStr, fenInitial);
}

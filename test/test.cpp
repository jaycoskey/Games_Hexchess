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

#include "board.h"
#include "move.h"
#include "util_hexchess.h"
#include "variant.h"

using std::cout;

using hexchess::core::Board;
using hexchess::core::Color;
using hexchess::core::Glinski;
using hexchess::core::Move;
using hexchess::core::Moves;
using hexchess::core::PieceType;
using hexchess::core::Size;


int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    auto result = RUN_ALL_TESTS();
    return result;
}

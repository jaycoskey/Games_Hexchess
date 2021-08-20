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

#include "board.h"
#include "move.h"
#include "util_hexchess.h"
#include "variant.h"

using std::cout;

using std::vector;

using hexchess::core::Board;
using hexchess::core::Color;
using hexchess::core::Glinski;
using hexchess::core::HexRay;
using hexchess::core::HexRays;
using hexchess::core::Index;
using hexchess::core::Indices;
using hexchess::core::Move;
using hexchess::core::Moves;
using hexchess::core::Short;


/// \brief Test: Find the move count of Leaper over all starting points.
Short move_count_leaper(const vector<Indices>& destTable, bool verbose=false) {
    Short result = 0;

    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        result += destTable[index].size();
    }
    return result;
}

/// \brief Test: Find the move count of a Slider over all starting points.
///
/// Note: As of 2021-08-19, this returns smaller values than expected.
Short move_count_slider(const vector<HexRays<Glinski>>& slideRayTable, bool verbose=false) {
    Short result = 0;

    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        for (const HexRay<Glinski>& ray : slideRayTable[index]) {
            result += ray.size();
        }
    }
    return result;
}

/// \brief Test: Test the count of all King moves over all starting points.
void test_move_count_king(bool verbose=false) {
    Short move_count = move_count_leaper(Glinski::kingDests, verbose);

    if (verbose) {
        cout << "King:   Move count = " << move_count << "\n";
    }
    // TODO: assert(move_count == ??);
}

/// \brief Test: Test the count of all Queen moves over all starting points.
void test_move_count_queen(bool verbose=false) {
    Short move_count = move_count_slider(Glinski::queenRays, verbose);

    if (verbose) {
        cout << "Queen:  Move count = " << move_count << "\n";
    }
    // TODO: assert(move_count == ??);
}

/// \brief Test: Test the count of all Rook moves over all starting points.
void test_move_count_rook(bool verbose=false) {
    Short move_count = move_count_slider(Glinski::rookRays, verbose);

    if (verbose) {
        cout << "Rook:   Move count = " << move_count << "\n";
    }
    // TODO: assert(move_count == ??);
}

/// \brief Test: Test the count of all Bishop moves over all starting points.
void test_move_count_bishop(bool verbose=false) {
    Short move_count = move_count_slider(Glinski::bishopRays, verbose);

    if (verbose) {
        cout << "Bishop: Move count = " << move_count << "\n";
    }
    // TODO: assert(move_count == ??);
}

/// \brief Test: Test the count of all Knight moves over all starting points.
void test_move_count_knight(bool verbose=false) {
    Short move_count = 0;
    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        move_count += Glinski::knightDests[index].size();
    }
    // TODO: assert(move_count == ??);
}

/// \brief Test: Test the count of all legal starting moves.
void test_move_count_board(bool verbose=false) {
    Board<Glinski> b{};  // Initial game layout
    Moves bMoves = b.getLegalMoves(Color::Black);
    Moves wMoves = b.getLegalMoves(Color::White);

    if (verbose) {
        cout << "Black moves from initial board layout: " << bMoves.size() << "\n";
        cout << "White moves from initial board layout: " << wMoves.size() << "\n";
    }
    Short kCount = 2;
    Short qCount = 6;
    Short rCount = 6;
    Short bCount = 12;
    Short nCount = 8;
    Short pCount = 17;
    Short expectedCount = kCount + qCount + rCount + bCount + nCount + pCount;
    assert(expectedCount == 51);

    assert(bMoves.size() == expectedCount);
    assert(wMoves.size() == expectedCount);
}


int main(int argc, char *argv[]) {
    test_move_count_king(true);
    test_move_count_queen(true);
    test_move_count_rook(true);
    test_move_count_bishop(true);
    test_move_count_knight(true);

    test_move_count_board(true);
}

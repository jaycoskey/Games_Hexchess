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
using hexchess::core::PieceType;
using hexchess::core::Short;
using hexchess::core::Size;


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
///
///     Kings @ inner 37 cells have 12 moves each (444)
///     Next layer: 18*11 + 6 * 10 (= 258)
///     Outermost layer: 24 * 7 + 6 * 5 (= 198)
TEST(MoveTest, MoveCountKing) {
    bool verbose = true;

    Short move_count = move_count_leaper(Glinski::kingDests, verbose);
    if (verbose) {
        cout << "King:   Move count = " << move_count << "\n";
    }
    ASSERT_EQ(move_count, 900);
}

/// \brief Test: Test the count of all Queen moves over all starting points.
TEST(MoveTest, MoveCountQueen) {
    bool verbose = true;

    Short move_count = move_count_slider(Glinski::queenRays, verbose);
    if (verbose) {
        cout << "Queen:  Move count = " << move_count << "\n";
    }
    ASSERT_EQ(move_count, 900);
}

/// \brief Test: Test the count of all Rook moves over all starting points.
TEST(MoveTest, MoveCountRook) {
    bool verbose = true;

    Short move_count = move_count_slider(Glinski::rookRays, verbose);
    if (verbose) {
        cout << "Rook:   Move count = " << move_count << "\n";
    }
    ASSERT_EQ(move_count, 480);
}

/// \brief Test: Test the count of all Bishop moves over all starting points.
TEST(MoveTest, MoveCountBishop) {
    bool verbose = true;

    Short move_count = move_count_slider(Glinski::bishopRays, verbose);
    if (verbose) {
        cout << "Bishop: Move count = " << move_count << "\n";
    }
    ASSERT_EQ(move_count, 420);
}

/// \brief Test: Test the count of all Knight moves over all starting points.
///
/// Knights at innermost 19 (1+6+12) cells have 12 moves each (=228)
///     Next layer (18 cells): 12 * 10 + 6 * 8 (=168)
///     Next layer (24 cells): 6 * 8 + 12 * 7 + 6 * 6  (=168)
///     Outermost layer (30 cells): 12 * 6 + 12 * 5 + 6 * 4 (=156)
TEST(MoveTest, MoveCountKnight) {
    bool verbose = true;

    Short move_count = 0;
    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        move_count += Glinski::knightDests[index].size();
    }
    if (verbose) {
        cout << "Knight move count=" << move_count << "\n";
    }
    ASSERT_EQ(move_count, 720);
}

/// \brief Test: Test the count of all legal starting moves.
TEST(MoveTest, MoveCountBoard) {
    bool verbose = true;

    Board<Glinski> b{"Test_MoveCountBoard", false};  // Initial game layout
    b.initialize(Glinski::fenInitial);
    Moves wMoves = b.getLegalMoves(Color::White);
    Size kCount = 0;
    Size qCount = 0;
    Size rCount = 0;
    Size bCount = 0;
    Size nCount = 0;
    Size pCount = 0;
    for (const Move& move : wMoves) {
        switch (move.pieceType()) {
            case PieceType::King:   kCount++; break;
            case PieceType::Queen:  qCount++; break;
            case PieceType::Rook:   rCount++; break;
            case PieceType::Bishop: bCount++; break;
            case PieceType::Knight: nCount++; break;
            case PieceType::Pawn:   pCount++; break;
            default: break;
        }
    }
    Size kExp = 2;  ASSERT_EQ(kCount, kExp);
    Size qExp = 6;  ASSERT_EQ(qCount, qExp);
    Size rExp = 6;  ASSERT_EQ(rCount, rExp);
    Size bExp = 12; ASSERT_EQ(bCount, bExp);
    Size nExp = 8;  ASSERT_EQ(nCount, nExp);
    Size pExp = 17; ASSERT_EQ(pCount, pExp);
    if (verbose) {
        cout << "Initial King moves="   << kCount << ((kCount == kExp) ? "" : "!= expected\n");
        cout << "Initial Queen moves="  << qCount << ((qCount == qExp) ? "" : "!= expected\n");
        cout << "Initial Rook moves="   << rCount << ((rCount == rExp) ? "" : "!= expected\n");
        cout << "Initial Bishop moves=" << bCount << ((bCount == bExp) ? "" : "!= expected\n");
        cout << "Initial Knight moves=" << nCount << ((nCount == nExp) ? "" : "!= expected\n");
        cout << "Initial Pawn moves="   << pCount << ((pCount == pExp) ? "" : "!= expected\n");
    }
    Size expectedCount = kExp + qExp + rExp
                       + bExp + nExp + pExp;
    ASSERT_EQ(wMoves.size(), expectedCount);
}

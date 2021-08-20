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

#include <algorithm>
#include <iostream>
#include <set>
#include <type_traits>
#include <vector>

#include "bitboard.h"
#include "geometry.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"


using std::cout;
using std::string;

using std::vector;

using hexchess::core::Bitboard;
using hexchess::core::BoardDir;
using hexchess::core::CellShade;
using hexchess::core::Color;
using hexchess::core::Glinski;
using hexchess::core::HexDir;
using hexchess::core::HexPos;
using hexchess::core::Index;
using hexchess::core::PieceType;


/// \brief Test: For each Player, the Bishop starting positions have three different cell shades.
void test_bitboard_bishopShades() {
    Bitboard<Glinski> bb{};

    for (Color c : {Color::Black, Color::White}) {
        vector<Index> bishopCellIndices{};
        for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
            if (bb.isBishopAtIndex(index, c)) {
                bishopCellIndices.push_back(index);
            }
        }
        assert(bishopCellIndices.size() == 3);

        vector<CellShade> bishopCellShades{};
        transform_push_back_if(
            bishopCellIndices.begin(),
            bishopCellIndices.end(),
            bishopCellShades,
            [&bb,c](Index bIndex) -> bool /* const */ {
                return !bb.isEmpty(bIndex)
                    && bb.getPieceTypeAt(bIndex, c) == PieceType::Bishop;
                },
            [](Index index) {
                return Glinski::cellShade(index);
                }
            );

        // There are three Shades.
        assert(bishopCellShades.size() == 3);

        // All three Shades are distinct.
        assert(std::set(bishopCellShades.cbegin(), bishopCellShades.cend()).size() == 3);
    }
}

/// \brief Test: The FEN of a newly constructed board matches the (standard) initial FEN.
///
/// For more information on Forsyth-Edwards Notation (FEN), see Wikipedia.
void test_bitboard_fen(bool verbose=false) {
    Bitboard<Glinski> bb{};
    if (verbose) {
        cout << "========================================\n";
        cout << bb.bitboard_string() << "\n";
        cout << "========================================\n";
    }
    string fen_init = decltype(bb)::V::fenInitial;
    string fen_board = fen_init.substr(0, fen_init.find(' '));
    if (verbose) {
        cout << "FEN initial = " << fen_init << "\n";
        cout << "FEN(board)  = " << bb.fen_string() << "\n";
    }
    assert(bb.fen_string() == fen_board);
}

/// \brief Test: All of a Knight's moves land on a cell differently shaded than the starting cell.
void test_bitboard_knightMoves() {
    Index centerIndex = 45;
    HexPos centerPos = Glinski::indexToPos(centerIndex);
    CellShade centerShade = Glinski::cellShade(centerPos);

    vector<Index> knightDests{};
    for (const HexDir& knightLeapDir : BoardDir::knightLeapDirs) {
        if (Glinski::isOnBoard(centerPos + knightLeapDir)) {
            knightDests.push_back(Glinski::posToIndex(centerPos + knightLeapDir));
        }
    }
    assert(knightDests.size() == 12);

    std::vector<CellShade> knightDestShades{};
    for (Index knightDest : knightDests) {
        knightDestShades.push_back(Glinski::cellShade(knightDest));
    };

    assert(std::all_of(
        knightDestShades.cbegin(),
        knightDestShades.cend(),
        [centerShade](CellShade shade) { return shade != centerShade; }
        ));
}

/// \brief Test: The piece counts for a starting position match the expected values.
void test_bitboard_pieceCount(bool verbose=false) {
    Bitboard<Glinski> bb{};

    for (Color c : {Color::Black, Color::White}) {
        if (verbose) {
            cout << (c == Color::Black ? "Black" : "White") << " pieces:\n";
            cout << "\tTotal:   " << bb.anyPieceBits(c).count() << "\n";
            cout << "\tKings:   " << bb.kingBits(c).count()   << "\n";
            cout << "\tQueen:   " << bb.queenBits(c).count()  << "\n";
            cout << "\tRook:    " << bb.rookBits(c).count()   << "\n";
            cout << "\tBishop:  " << bb.bishopBits(c).count() << "\n";
            cout << "\tKnights: " << bb.knightBits(c).count() << "\n";
            cout << "\tPawns:   " << bb.pawnBits(c).count()   << "\n";
        }

        assert(bb.anyPieceBits(c).count() == 18);
        assert(bb.kingBits(c).count() == 1);
        assert(bb.queenBits(c).count() == 1);
        assert(bb.rookBits(c).count() == 2);
        assert(bb.bishopBits(c).count() == 3);
        assert(bb.knightBits(c).count() == 2);
        assert(bb.pawnBits(c).count() == 9);
    }
}

/// \brief Calls to addPiece and removePiece work as expected.
void test_bitboard_piecePlacement() {
    Bitboard<Glinski> bb{};
    Index centerIndex = 45;
    assert(bb.isEmpty(centerIndex));

    bb.addPiece(centerIndex, Color::Black, PieceType::Knight);
    assert(bb.getColorAt(centerIndex) == Color::Black);
    assert(bb.getPieceTypeAt(centerIndex) == PieceType::Knight);

    bb.removePiece(centerIndex, Color::Black, PieceType::Knight);
    assert(bb.isEmpty(centerIndex));

    assert(bb.zobristHash() == 0x712bf5ea63571cf5);
};


int main(int argc, char *argv[]) {
    test_bitboard_pieceCount();
    test_bitboard_bishopShades();
    test_bitboard_fen();
    test_bitboard_knightMoves();
    test_bitboard_piecePlacement();
}

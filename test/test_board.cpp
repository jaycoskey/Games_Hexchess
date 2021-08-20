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

#include "board.h"
#include "geometry.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"

using std::cout;
using std::string;

using std::vector;

using hexchess::transform_push_back_if;

using hexchess::core::Board;
using hexchess::core::BoardDir;
using hexchess::core::CellShade;
using hexchess::core::Color;
using hexchess::core::Glinski;
using hexchess::core::HexDir;
using hexchess::core::HexPos;
using hexchess::core::Index;
using hexchess::core::PieceType;
using hexchess::core::Short;


/// \brief Test: For each Player, the Bishop starting positions have three different cell shades.
void test_board_bishopShades() {
    Board<Glinski> b{};

    for (Color c : {Color::Black, Color::White}) {
        vector<Index> bishopCellIndices{};
        for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
            if (b.isBishopAtIndex(index, c)) {
                bishopCellIndices.push_back(index);
            }
        }
        assert(bishopCellIndices.size() == 3);

        vector<CellShade> bishopCellShades{};
        transform_push_back_if(
            bishopCellIndices.begin(),
            bishopCellIndices.end(),
            bishopCellShades,
            [&b,c](Index k) -> bool /* const */ {
                return !b.isEmpty(k)
                    && b.getPieceTypeAt(k, c) == PieceType::Bishop;
                },
            [](Index k) {
                return Glinski::cellShade(k);
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
void test_board_fen(bool verbose=false) {
    Board<Glinski> b{};
    if (verbose) {
        cout << "========================================\n";
        cout << b.board_string() << "\n";
        cout << "========================================\n";
    }
    string fen_init = decltype(b)::V::fenInitial;
    string fen_derived = b.fen_string();
    if (verbose) {
        cout << "FEN initial = \"" << fen_init    << "\"\n";
        cout << "FEN(board)  = \"" << fen_derived << "\"\n";
    }
    assert(fen_derived == fen_init);
}

/// \brief Test: All of a Knight's moves land on a cell differently shaded than the starting cell.
void test_board_knightMoves() {
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
void test_board_pieceCount(bool verbose=false) {
    Board<Glinski> b{};

    for (Color c : {Color::Black, Color::White}) {
        if (verbose) {
            cout << (c == Color::Black ? "Black" : "White") << " pieces:\n";
            cout << "\tTotal:   " << b.anyPieceBits(c).count() << "\n";
            cout << "\tKings:   " << b.kingBits(c).count()   << "\n";
            cout << "\tQueen:   " << b.queenBits(c).count()  << "\n";
            cout << "\tRook:    " << b.rookBits(c).count()   << "\n";
            cout << "\tBishop:  " << b.bishopBits(c).count() << "\n";
            cout << "\tKnights: " << b.knightBits(c).count() << "\n";
            cout << "\tPawns:   " << b.pawnBits(c).count()   << "\n";
        }

        assert(b.anyPieceBits(c).count() == 18);
        assert(b.kingBits(c).count() == 1);
        assert(b.queenBits(c).count() == 1);
        assert(b.rookBits(c).count() == 2);
        assert(b.bishopBits(c).count() == 3);
        assert(b.knightBits(c).count() == 2);
        assert(b.pawnBits(c).count() == 9);
    }
}

/// \brief Calls to addPiece and removePiece work as expected.
void test_board_piece_placement() {
    Board<Glinski> b{};
    Index centerIndex = 45;
    assert(b.isEmpty(centerIndex));

    b.addPiece(centerIndex, Color::Black, PieceType::Knight);
    assert(b.getColorAt(centerIndex) == Color::Black);
    assert(b.getPieceTypeAt(centerIndex) == PieceType::Knight);

    b.removePiece(centerIndex, Color::Black, PieceType::Knight);
    assert(b.isEmpty(centerIndex));

    assert(b.zobristHash() == 0x712bf5ea63571cf5);
};


int main(int argc, char *argv[]) {
    test_board_pieceCount();
    test_board_bishopShades();
    test_board_fen();
    test_board_knightMoves();
    test_board_piece_placement();
}

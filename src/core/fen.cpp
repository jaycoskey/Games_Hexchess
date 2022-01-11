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
#include <cctype>
#include <cstdlib>

#include <sstream>
#include <string>
#include <vector>

#include "fen.h"
#include "variant.h"

using std::cout;
using std::string;

using std::vector;


namespace hexchess::core {

// ========================================
// Static methods

/// \brief Parses the first part of the FEN string to find piece placements
///
/// \todo Add sanity check test that the FEN row lengths string those in Glinski::fenRowLengths.
template<>
const PiecesSparse Fen<Glinski>::fen_board_parse(const string& fenStr, bool verbose) {
    // Grab the FEN string up to the first space, in case a full FEN string was passed in
    string fenBoard = fenStr.substr(0, fenStr.find(' ', 0));
    const vector<string>& rows = split('/', fenBoard);

    PiecesSparse result{V::CELL_COUNT, std::nullopt};
    Index fenOrder = 0;
    Index rowNum = -1;
    for (const string& row : rows) {
        rowNum++;
        Index cellsRemainingInRow = Glinski::fenRowLengths[rowNum];
        if (verbose) {
            cout << "Row #" << rowNum << ": " << row << "\n";
        }
        for (const char ch : row) {
            if (verbose) {
                cout << "\tChar=" << ch << ". Cells remaining=" << cellsRemainingInRow << "\n";
            }
            if (isdigit(ch)) {
                // TODO: Modify here as needed to support variants with rows 10 cells wide.
                Short gap = ch - '0';
                fenOrder += gap;
                cellsRemainingInRow -= gap;
                assert(cellsRemainingInRow >= 0);
                continue;
            } else {
                // The current character represents a piece.
                Index index = V::fenOrderToIndex[fenOrder];
                result[index] = std::make_optional(piece_fen_parse(ch));
                fenOrder++;
                cellsRemainingInRow--;
            }
        }
    }
    // Note: FEN board string does not end with '/'
    assert(rowNum == Glinski::ROW_COUNT - 1);
    return result;
}

template<>
const string Fen<Glinski>::fen_board_string(const PiecesSparse& pieces, bool verbose) {
    std::ostringstream oss;

    int blankCount = 0;
    int rowNum = 0;
    int cellsRemainingInRow = Glinski::fenRowLengths[rowNum];

    for (Index index : V::fenOrderToIndex) {
        if (!pieces[index].has_value()) {
            blankCount++;
            if (verbose) { cout << '.'; }
        } else {
            if (blankCount > 0) {
                oss << blankCount;
                if (verbose) { cout << blankCount; }
                blankCount = 0;
            }
            auto [c, pt] = pieces[index].value();
            oss << piece_fen_string(c, pt);
            if (verbose) { cout << piece_fen_string(c, pt); }
        }
        cellsRemainingInRow--;

        if (cellsRemainingInRow == 0) {
            if (blankCount > 0) {
                oss << blankCount;
                if (verbose) { cout << blankCount; }
                blankCount = 0;
            }
            rowNum++;
            if (rowNum < Glinski::ROW_COUNT) {
                oss << '/';
                if (verbose) { cout << '/'; }
                cellsRemainingInRow = Glinski::fenRowLengths[rowNum];
            }
        }
    }
    if (verbose) { cout << '\n'; }
    return oss.str();
}

// ========================================
// Constructor from FEN string

template<>
Fen<Glinski>::Fen(const string& s) {
    vector<string> fenParts = split(' ', s);
    piecesSparse = Fen<Glinski>::fen_board_parse(fenParts[0]);
    assert(piecesSparse.size() == V::CELL_COUNT);
    mover = fenParts[1] == "b" ? Color::Black : Color::White;
    // [2]: CastlingBits // \todo Add support for castling
    optEpIndex = fenParts[3] == "-"
                     ? std::nullopt
                     : std::make_optional(V::cellNameToIndex(fenParts[3]));
    currentCounter = std::stoi(fenParts[4]) - 1;  // currentCounter is zero-based
    // Note: Full move counter is derived from currentCounter
}

// ========================================
// Instance method(s)

template<>
const string Fen<Glinski>::fen_string() const {
    std::ostringstream oss;

    assert(piecesSparse.size() == V::CELL_COUNT);
    oss << Fen<V>::fen_board_string(piecesSparse) << " "
        << (mover == Color::Black ? 'b' : 'w') << " "
        << "- "  /* \todo Support for castling */
        << (optEpIndex == std::nullopt
               ? "-"
               : Glinski::cellName(optEpIndex.value()))
        << " "
        << currentCounter + 1 << " "  // currentCounter is zero-based
        << (currentCounter + 2) / 2
        ;

    return oss.str();
}

}  // namespace hexchess::core

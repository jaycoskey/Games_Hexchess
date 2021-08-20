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

#pragma once

#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::core {

template <typename Variant>
struct Fen {
// public:
    typedef Variant V;

    static const PiecesSparse fen_board_parse(const std::string& fenStr, bool verbose=false);
    static const std::string fen_board_string(const PiecesSparse& pieces, bool verbose=false);

    Fen(const std::string& fenStr);

    Fen(const PiecesSparse& pieces,
        Color c,
        // TODO: Support castling: V::Bits castlingBits,
        OptIndex oep,
        HalfMoveCounter counter
        // Full move counter is not needed
        )
        : piecesSparse{pieces},
          mover{c},
          optEpIndex{oep},
          currentCounter{counter}
    { }

    // const PiecesSparse& pieces() const { return _piecesSparse; }
    // Color mover() const { return _mover; }
    // const OptIndex& optEpIndex() const { return _optEpIndex; }
    // const HalfMoveCounter currentCounter() const { return _currentCounter; }

    const std::string fen_string();

// private:
    PiecesSparse piecesSparse;
    Color mover;
    // V::Bits _castlingBits;  /// \todo Add support for castling to class Fen
    OptIndex optEpIndex;
    HalfMoveCounter currentCounter;
};

}  // namespace hexchess::core

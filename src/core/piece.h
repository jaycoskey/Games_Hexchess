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

#include <cassert>

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "geometry.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::core {

using OptPieceType = std::optional<PieceType>;
using PieceValue = float;


// template<typename Variant>
class Piece {
public:
    // typedef Variant V;

    static PieceValue pieceValue(PieceType pt);

    Piece(Index index, Color color, PieceType pt);

    Index index() const { return _index; }
    Color color() const { return _color; }
    PieceType pieceType() const { return _pieceType; }
    bool isCaptured() const { return _isCaptured; }

    // HexCoord hex0() const { return V::indexToHex0(_index); }
    // HexCoord hex1() const { return V::indexToHex1(_index); }
    // CellShade const cellShade() { return V::cellShade(_index); }

    bool hasMoved(HalfMoveCounter currentCounter) const;
    void moveTo(Index index, HalfMoveCounter currentCounter);

    std::string ascii() { return piece_string(_color, _pieceType); }
    std::string unicode() { return piece_unicode(_color, _pieceType); }

private:
    Index     _index;
    Color     _color;
    PieceType _pieceType;
    bool      _isCaptured;
    std::vector<HalfMoveCounter> _whenMoved;  // Needed for Castling & Undo to work together (Kings & Rooks only)

    // template<typename Var>
    friend std::ostream &operator<<(std::ostream &os, const Piece &piece);
};

}  // namespace hexchess::core

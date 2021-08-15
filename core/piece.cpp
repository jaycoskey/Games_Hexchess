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

#include "piece.h"
#include "util_hexchess.h"


namespace hexchess::core {

void Piece::moveTo(Index index, HalfMoveCounter currentCounter) {
    _index = index;
    _whenMoved.push_back(currentCounter);
}

// template<>
bool Piece::hasMoved(HalfMoveCounter currentCounter) const {
    assert(_pieceType == PieceType::King || _pieceType == PieceType::Rook);
    if (_whenMoved.empty()) { return false; }
    return _whenMoved[0] < currentCounter;
}

}  // namespace hexchess::core

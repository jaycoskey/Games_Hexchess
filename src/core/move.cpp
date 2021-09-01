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

#include <iostream>

#include "move.h"
#include "variant.h"

using std::string;


namespace hexchess::core {

Move& Move::operator=(const Move& other) {
    if (this != &other) {
        _mover = other._mover;
        _pieceType = other._pieceType;
        _from = other._from;
        _to = other._to;
        _moveEnum = other._moveEnum;
        _optCaptured = other._optCaptured;
        _optPromotedTo = other._optPromotedTo;
        _optCheckEnum = other._optCheckEnum;

        // _mover = other.mover();
        // _pieceType = other.pieceType();
        // _from = other.from();
        // _to = other.to();
        // _moveEnum = other.moveEnum();
        // _optCaptured = other.optCaptured();
        // _optPromotedTo = other.optPromotedTo();
        // _checkStatus = other.checkStatus();
    }
    return *this;
}

bool Move::isCheck() const {
    assert(_optCheckEnum.has_value());
    return _optCheckEnum.value() == CheckEnum::Check;
}

bool Move::isCheckmate() const {
    assert(_optCheckEnum.has_value());
    return _optCheckEnum.value() == CheckEnum::Checkmate;
}

bool Move::isProgressMove() const {
    return _pieceType == PieceType::Pawn || _optCaptured != std::nullopt;
}

const std::string Move::move_lan_string() const {
    std::string result{};

    if (_moveEnum == MoveEnum::Castling) {
        // TODO: Loop over castlings. On a match, add code & break
    } else {
        result += piece_type_string(_pieceType)
                  + Glinski::cellName(_from)
                  + (isCapture() ? "x" : "-")
                  + Glinski::cellName(_to);
    }
    if (isEnPassant()) { result += "ep"; }
    if (isPromotion()) { result += string{'='} + piece_type_string(_optPromotedTo.value()); }

    if (isCheck()) { result += "+"; }
    if (isCheckmate()) { result += "#"; }
    return result;
}

bool operator==(const Move& a, const Move& b) {
    bool result = a._mover == b._mover
                && a._pieceType == b._pieceType
                && a._from == b._from
                && a._to == b._to
                && a._moveEnum == b._moveEnum
                && a._optCaptured == b._optCaptured
                && a._optPromotedTo == b._optPromotedTo
                && a._optCheckEnum == b._optCheckEnum;
    return result;
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << move.move_lan_string();
    return os;
}

}  // namespace hexchess::core

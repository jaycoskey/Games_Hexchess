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

// Returns false if it has not been determined
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

// Note: The platform only sets _optCheckEnum after a call to moveExec.
//       Only then should this be called with doChecks=true.
const std::string Move::move_pgn_string(bool doChecks) const {
    assert (_from >= 0 && _from < Glinski::CELL_COUNT);
    std::ostringstream oss;

    if (_moveEnum == MoveEnum::Castling) {
        // TODO: Loop over castlings. On a match, add code & break
    } else {
        oss << piece_type_string(_pieceType);
        oss << Glinski::cellName(_from);
        oss << (isCapture()
                   ? "x"
                   //  (string("x(")
                   //     + piece_type_string(_optCaptured.value())
                   //     + ")")
                   : "-");
        oss << Glinski::cellName(_to);
    }
    if (isEnPassant()) {
        oss << "ep";
    }
    if (isPromotion()) {
        oss << '=' << piece_type_string(_optPromotedTo.value());
    }
    if (doChecks) {
        if (isCheck()) { oss << "+"; }
        if (isCheckmate()) { oss << "#"; }
    }
    return oss.str();
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
    os << move.move_pgn_string();
    return os;
}

}  // namespace hexchess::core

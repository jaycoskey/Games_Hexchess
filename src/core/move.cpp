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
#include "zobrist.h"

using std::cout;
using std::string;


namespace hexchess::core {

const std::string move_enum_string(MoveEnum me) {
    static const std::map<MoveEnum, std::string> me2s {
        { MoveEnum::Simple,        "Simple"        },
        { MoveEnum::Castling,      "Castling"      },
        { MoveEnum::EnPassant,     "EnPassant"     },
        { MoveEnum::PawnPromotion, "PawnPromotion" }
    };
    return me2s.at(me);
}
std::ostream& operator<<(std::ostream& os, MoveEnum me) {
    return (os << move_enum_string(me));
}

Move& Move::operator=(const Move& other) {
    if (this != &other) {
        _mover = other._mover;
        _pieceType = other._pieceType;
        _from = other._from;
        _to = other._to;
        _moveEnum = other._moveEnum;
        _optCaptured = other._optCaptured;
        _optPromotedTo = other._optPromotedTo;
    }
    return *this;
}

// Returns false if it has not been determined
bool Move::isCheck() const {
    if (!_optCheckEnum.has_value()) {
        throw std::logic_error{"Move::isCheck called for a move without this value set. Try first calling Board::setLegalMoveCheckEnums."};
    }
    return _optCheckEnum.value() == CheckEnum::Check;
}

bool Move::isCheckmate() const {
    assert(_optCheckEnum.has_value());
    return _optCheckEnum.value() == CheckEnum::Checkmate;
}

MHash Move::getHash() const {
    static const Short colorCount = 2;
    static const Short pieceTypeCount = 6;
    static const Short cellCount = Glinski::CELL_COUNT;
    static const Short moveEnumCount = 4;
    // static const Short promotionPieceTypeCount = 4;

    static std::map<Color, Short> c2zi {
        { Color::Black, 0 }, {Color::White, 1}
    };
    static std::map<PieceType, Short> pt2zi {
        { PieceType::King,   0 },
        { PieceType::Queen,  1 },
        { PieceType::Rook,   2 },
        { PieceType::Bishop, 3 },
        { PieceType::Knight, 4 },
        { PieceType::Pawn,   5 }
    };
    static std::map<MoveEnum, Short> me2zi {
        { MoveEnum::Simple,        0 },
        { MoveEnum::Castling,      1 },
        { MoveEnum::EnPassant,     2 },
        { MoveEnum::PawnPromotion, 3 }
    };

    Short zi[7];
    zi[0] = c2zi.at(_mover);
    zi[1] = colorCount + pt2zi.at(_pieceType);
    zi[2] = colorCount + pieceTypeCount + _from;
    zi[3] = colorCount + pieceTypeCount + cellCount + _to;
    zi[4] = colorCount + pieceTypeCount + 2 * cellCount
                 + me2zi.at(_moveEnum);
    zi[5] = colorCount + pieceTypeCount + 2 * cellCount + moveEnumCount
                 + (_optCaptured.has_value() ? pt2zi.at(_optCaptured.value()) : 0);
    zi[6] = colorCount + 2 * pieceTypeCount + 2 * cellCount + moveEnumCount
                 + (_optPromotedTo.has_value() ? pt2zi.at(_optPromotedTo.value()) : 0);

    MHash result = 0;
    for (Short k = 0; k < 7; ++k) {
        result += zi[k];
    }
    return result;
}

bool Move::isProgressMove() const {
    return _pieceType == PieceType::Pawn || _optCaptured != std::nullopt;
}

// Note: The platform only sets _optCheckEnum after a call to moveExec.
//       Only then should this be called with doChecks=true.
const std::string Move::move_pgn_string(bool doChecks) const {
    bool doShowExtras = true;

    if (_from < 0 || _from >= Glinski::CELL_COUNT) {
	string msg{"Move::isCheck called for a move without the _from value set. Try first calling Board::setLegalMoveCheckEnums."};
        throw std::logic_error{msg};
    }
    assert (_from >= 0 && _from < Glinski::CELL_COUNT);
    std::ostringstream oss;

    if (_moveEnum == MoveEnum::Castling) {
        // TODO: Loop over castlings. On a match, add code & break
    } else {
        if (doShowExtras) {
            oss << mover() << "-";
        }
        oss << piece_type_string(_pieceType);
        oss << Glinski::cellName(_from);
        if (doShowExtras) {
            oss << (isCapture()
                        ? string{"x("}
                            + piece_type_string(_optCaptured.value())
                            + ")"
                        : "-"
                   );
        } else {
            oss << (isCapture() ? "x" : "-");
        }
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

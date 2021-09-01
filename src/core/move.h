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

#include <optional>
#include <sstream>

#include "util.h"
#include "util_hexchess.h"


namespace hexchess::core {

class Move;

// Should Moves contain more structure?
// E.g., map<PieceType, vector<Move>>?
using Moves = std::vector<Move>;
using OptMove = std::optional<Move>;
using OptMoves = std::optional<Moves>;

using OptPieceType = std::optional<PieceType>;


/// \brief The different types of Check that a Move can have.
///
/// When a Move is first created, its check type is set to Unknown.
/// If the opponent is later determined to not be in check Later, it is set to NoCheck.
/// Otherwise, the check type is set to Check or Checkmate, as appropriate.
enum class CheckEnum {
    None,
    Check,
    Checkmate
};
using OptCheckEnum = std::optional<CheckEnum>;

/// \brief The different types of Check that a Move can have.
///
/// These are enumerated to simplify the Undo of moves.
/// Note: This list of move types is variant-dependent.
/// Note: PawnDoubleStep could also be added as a Move type.
enum class MoveEnum {
    Simple,

    Castling,
    EnPassant,  // EnPassant = the capture move, not the Pawn double-advance
    PawnPromotion
};

/// \brief The record of a player Move.
///
/// This class contain info to execute or undo a move.
class Move {
public:
    Move(Color mover,
        PieceType pt,
        Index from,
        Index to,
        MoveEnum mt=MoveEnum::Simple,
        OptPieceType optCaptured=std::nullopt,
        OptPieceType optPromotedTo=std::nullopt,
        OptCheckEnum optCheckEnum=std::nullopt
        )
        : _mover{mover},
          _pieceType{pt},
          _from{from},
          _to{to},
          _moveEnum{mt},
          _optCaptured{optCaptured},
          _optPromotedTo{optPromotedTo},
          _optCheckEnum{optCheckEnum}
    { }

    Move(const Move& other) = default;
    ~Move() {};
    Move& operator=(const Move& other);

    Color mover() const         { return _mover; }
    PieceType pieceType() const { return _pieceType; }
    Index from() const          { return _from; }
    Index to() const            { return _to; }
    MoveEnum moveEnum() const   { return _moveEnum; }

    const OptPieceType optCaptured()   const { return _optCaptured; };
    const OptPieceType optPromotedTo() const { return _optPromotedTo; };

    bool hasCheckEnum() const { return _optCheckEnum.has_value(); }

    bool isCapture() const    { return _optCaptured != std::nullopt; }
    bool isCastling() const   { return _moveEnum == MoveEnum::Castling; }

    // TODO: Implement setting of isCheck
    /// Note: Check and Checkmate are exclusive of each other. Checkmate is not a type of Check.
    bool isCheck() const;
    bool isCheckmate() const;
    bool isEnPassant() const  { return _moveEnum == MoveEnum::EnPassant; }

    /// \brief Used to determine whether to reset the nonProgressCounter.
    bool isProgressMove() const;
    bool isPromotion() const { return _moveEnum == MoveEnum::PawnPromotion; }

    const std::string move_lan_string() const;

private:
    Color _mover;                 ///< \brief The player moving
    PieceType _pieceType;         ///< \brief On castling, pieceType == PieceType::King
    Index _from;                  ///< \brief The beginning position
    Index _to;                    ///< \brief The ending position
    MoveEnum _moveEnum;           ///< \brief The move type, to simplify Undo
    OptPieceType _optCaptured;    ///< \brief The captured piece type (for Undo)
    OptPieceType _optPromotedTo;  ///< \brief The chosen promotion type
    OptCheckEnum _optCheckEnum;   ///< \brief The Check type: Unknown/NoCheck/Check/Checkmate

friend bool operator==(const Move& a, const Move& b);
};

// bool operator==(const Move& a, const Move& b);
std::ostream& operator<<(std::ostream& os, const Move& move);

}  // namespace hexchess::core)

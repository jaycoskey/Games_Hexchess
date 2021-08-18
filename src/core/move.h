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

#include "util.h"
#include "util_hexchess.h"


namespace hexchess::core {

class Move;
using Moves = std::vector<Move>;

using OptPieceType = std::optional<PieceType>;

class Piece;


/// \brief The different types of Check that a Move can have.
///
/// When a Move is first created, its check type is set to Unknown.
/// If the opponent is later determined to not be in check Later, it is set to NoCheck.
/// Otherwise, the check type is set to Check or Checkmate, as appropriate.
enum class CheckEnum {
    Unknown,
    NoCheck,
    Check,
    Checkmate
};

/// \brief The different types of Check that a Move can have.
///
/// These are enumerated to simplify the Undo of moves.
/// Note that this list of move types is variant-dependent.
enum class MoveEnum {
    Simple,

    Castling,
    EnPassant,
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
        Piece* capturedPiece=nullptr,
        OptPieceType optPromo=std::nullopt,
        CheckEnum checkType=CheckEnum::Unknown
        )
        : _mover{mover},
          _pieceType{pt},
          _from{from},
          _to{to},
          _moveEnum{mt},
          _capturedPiece{capturedPiece},
          _optPromotedTo{optPromo},
          _checkType{checkType}
    {};
    Move(const Move& other) = delete;
    ~Move() {};
    Move& operator=(const Move& other) = delete;

    /// \brief Used to determine whether to reset the nonProgressCounter.
    bool isProgressMove();

    /// \brief Returns the PGN of the game up to the current half-move.
    std::string pgnText();

private:
    Color _mover;                 ///< \brief The player moving
    PieceType _pieceType;         ///< \brief On castling, pieceType == PieceType::King
    Index _from;                  ///< \brief The start
    Index _to;                    ///< \brief The move destination
    MoveEnum _moveEnum;           ///< \brief The move type, to simplify Undo
    Piece* _capturedPiece;        ///< \brief The captured piece, to support Undo
    OptPieceType _optPromotedTo;  ///< \brief The chosen promotion type
    CheckEnum _checkType;         ///< \brief The Check type: Unknown/NoCheck/Check/Checkmate
};

}  // namespace hexchess::core

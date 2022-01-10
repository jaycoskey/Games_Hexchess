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

#include <iostream>
#include <optional>

#include "util_hexchess.h"


namespace hexchess::core {

/// \brief The condition that caused the game to end: checkmate, resignation, stalemate, etc.
///
/// Technically, there can be more than one enabling condition or trigger for a draw.
///     (For example, a given move can max out the maximum number of board repetitions
///     and non-progress moves---moves without a capture or Pawn move.)
/// This could be tracked with (OR-able) flags, but instead, in such cases,
/// we'll just choose one of the termination conditions as being "primary".
///
/// \todo [Long-term] Implement detection of a dead position.
/// \todo [Short-term] Implement detection of insufficient resources.
enum class Termination {
    None,                      ///< \brief Game ongoing, or hasn't been checked since the last move.
    Win_Checkmate,             ///< \brief Win_Proposed_Accepted
    // Win_Proposed_Accepeted  // \brief Player, during turn, requests Win. Opponent agrees.
    Win_Resign,                ///< \brief A Player resigned instead of moving.
    // Win_Timeout,            // \brief A Player won because their opponent ran out of time.
    // Win_Timeout_Claimed,    // \brief Use when claim came after both flags fell, or after an intermediate move, or unknown.
    // ====================
    Draw_3xBoardRepetition,    ///< In Glinski chess, 3x repetition causes automatic Draw
    Draw_50MoveRule,           ///< In Glinski chess, 50 moves without progress causes automatic Draw

    // Draw_5xRepetition,      ///< \brief Note: This ends the game automatically. No claim needed.
    // Draw_75MoveRule,        ///< \brief Note: This ends the game automatically. No claim needed.
    // Draw_Agreement,         // \brief One player proposes a Draw, and the other accepts.

    /// \brief 3x repetition. (Allows draw to be claimed, but does not automatically end the game.)
    // Draw_Claimed_3xRepetition,

    /// \brief 50 moves without capture or Pawn move. (Allows draw to be claimed, but does not automatically end the game.)
    // Draw_Claimed_50MoveRule,

    // \brief A game that cannot be won, due to arrangement of the pieces.
    // Draw_DeadPosition,

    /// \brief A game that cannot be won, due to which pieces are on the board. (In Glinski chess, KQ vs KR, KR vs K)
    Draw_InsufficientResources,
    Draw_Stalemate              ///< \brief Player has no moves available on their turn.
};

std::string termination_string(Termination t);

// ========================================

/// \brief Records the outcome of a game.
///
/// Note: Instances of this struct are only created when the game ends,
///       so there is no need for std::optional members.
/// If Termination is any type of win, then winner reflects the winner.
/// Note: In Glinski chess, a Stalemate earns 3/4 point for the last mover, and 1/4 for the other,
///       In that case, winner is the player who last moved.
struct GameOutcome {
public:
    GameOutcome(Termination term)
        : _termination{term},
          _optWinner{std::nullopt}
    {
        std::cout << "GameOutcome::GameOutcome (winner=nullopt): _termination="
                  << termination_string(term) << "\n";
    }

    GameOutcome(Termination term, Color winner)
        : _termination{term},
          _optWinner{std::make_optional(winner)}
    {
        std::cout << "GameOutcome::GameOutcome (winner!=nullopt): _termination="
                  << termination_string(term) << "\n";
    }

    bool isDraw() const;
    bool isStalemate() const;
    bool isWin() const;

    std::string game_outcome_reader_string(Color reader) const;
    std::string game_outcome_short_string() const;

    std::string game_outcome_score_string() const;
    Score score(Color c) const;
    Termination termination() const { return _termination; }
    Color winner() const {
        if (_optWinner == std::nullopt) {
            throw std::logic_error("GameOutcome::winner: No winner");
        }
        return _optWinner.value();
    }

    bool operator==(const GameOutcome& other) const;

private:
    Termination _termination;  ///< How the game ended, or None, if the game is still in play.
    OptColor _optWinner;  ///< The winner if termination reflects a Win or a draw due to Stalemate.
};

using OptGameOutcome = std::optional<GameOutcome>;

}  // namespace hexchess::core

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
#include <map>
#include <sstream>
#include <string>

#include "game_outcome.h"
#include "util_hexchess.h"

using std::cout;
using std::string;

using std::map;


namespace hexchess::core {

string termination_string(Termination t) {
    static map<Termination, string> t2s {
        { Termination::None,              "none"                        },
        { Termination::Win_Checkmate,     "checkmate"                   },
        { Termination::Win_Resign,        "resignation"                 },
        { Termination::Draw_3xBoardRepetition, "three-time board repetition" },
        { Termination::Draw_50MoveRule,   "50 moves without capature or Pawn move" },
        { Termination::Draw_InsufficientResources,
            "insufficient resources (i.e., pieces left cannot force a checkmate)" },
        { Termination::Draw_Stalemate,    "stalemate" }
    };
    return t2s[t];
}

bool GameOutcome::isDraw() const {
    return termination() == Termination::Draw_3xBoardRepetition
            || termination() == Termination::Draw_50MoveRule
            || termination() == Termination::Draw_InsufficientResources
            || termination() == Termination::Draw_Stalemate;
}

bool GameOutcome::isStalemate() const {
    return termination() == Termination::Draw_Stalemate;
}

bool GameOutcome::isWin() const {
    return termination() == Termination::Win_Checkmate
        || termination() == Termination::Win_Resign;
}

string GameOutcome::game_outcome_reader_string(Color reader) const {
    std::ostringstream oss;

    if (isWin()) {
        string winType = string{"by "} + termination_string(termination());
        bool isReaderWinner = reader == winner();
        if (isReaderWinner) {
            oss << "You won " << winType << (isReaderWinner ? "1" : ".");
        }
    } else if (isDraw()) {
        oss << "The game was a Draw due to " << termination_string(termination()) << ". ";
        // TODO: Add "winner" of Stalemate for Glinski chess
    } else {
        throw std::logic_error("game_outcome_reader_string: Unrecognized Termination value");
    }

    oss << "Your score is " << score(reader);

    return oss.str();
}

std::string GameOutcome::game_outcome_score_string() const {
    std::ostringstream oss;
    oss << score(Color::White) << "-" << score(Color::Black);
    return oss.str();
}

Score GameOutcome::score(Color c) const {
    if (isWin()) {
        return winner() == c ? 1 : 0;
    } else if (isStalemate()) {
        return c == winner() ? 0.75 : 0.25;
    } else /* Draw */ {
        assert(isDraw());
        return 0.5;
    }
}

bool GameOutcome::operator==(const GameOutcome& other) const {
    // Note: Can omit winner comparison on non-Stalemate Draws.
    return termination() == other.termination() && winner() == other.winner();
}

}  // namespace hexchess::core

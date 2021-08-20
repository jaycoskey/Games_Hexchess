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

#include <iostream>

#include "game.h"
#include "game_outcome.h"
#include "util_hexchess.h"

using std::string;


namespace hexchess::core {

template<>
std::shared_ptr<Player> Game<Glinski>::getPlayer(Color c) {
    return c == Color::Black ? _bPlayer : _wPlayer;
}

template<>
const GameOutcome& Game<Glinski>::play() {
    throw NotImplementedException{"Board<Glinski>::play"};
}

/// \todo Add Game::game_outcome_json_string()
template<>
const string Game<Glinski>::game_summary_string(const GameOutcome& outcome) const {
    std::ostringstream oss;

    oss << "Players=("
        <<   "White:\"" <<  _wPlayer->name() << "\", "
        <<   "Black:\"" <<  _bPlayer->name() << "\", "
        <<   "), "
        << "Outcome="
        ;

    if (outcome.isWin()) {
        oss << "Win for " << color_string(outcome.winner)
            << " by " << termination_string(outcome.termination);
    } else if (outcome.termination == Termination::Draw_Stalemate) {
        oss << "Stalemate by " << outcome.winner;
    } else /* Other form of Draw */ {
        oss << "Draw (" << termination_string(outcome.termination) << ")";
    }
    oss << ", ";
    oss << "Score(W)=" << outcome.score(Color::White) << ", ";
    oss << "Score(B)=" << outcome.score(Color::Black) << ".";

    return oss.str();
}

template<>
const string Game<Glinski>::pgn_string() const {
    throw NotImplementedException{"Board<Glinski>::pgn_string"};
}

}  // namespace hexchess::core

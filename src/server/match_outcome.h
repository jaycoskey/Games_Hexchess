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

namespace hexchess::core {

class MatchOutcome {
public:
    Match(Player& p1, Player& p2)
        : player1{p1},
          player2{p2},
          // -----
          game_count{0},
          game_p1_score{0},
          game_p2_score{0},
          // -----
          player1_draws_nonstalemate{0},
          player1_draws_stalemater{0},
          player1_losses{0},
          player1_wins{0}
    { }

    void play_match() {
        for (k = 1; k <= game_count
    }
    void test_game_counts() {
        Short total = outcome.player1_draws + outcome.wPlayer_losses + outcome.wPlayer_wins;
        assert(total == game_count);
    }

    Player& player1;
    Player& player2;

    Short gameCount;

    Real player1_score;
    Real player2_score;

    GameOutcomes _gameOutcomes{};

    Short getCount(player1, Termination t)
    Short player1_draws_nonstalemate;
    Short player1_losses;
    Short player1_wins;

    Short player1_stalemated;
    Short player1_other_draws;
};

}  // namespace hexchess::core

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

class Match {
public:
    Match(const Player& p1, const Player p2, Short gameCount)
        : player1 {p1},
          player2 {p2}
    {
        // Each player should play as Black the same number of times as White.
        assert(gameCount % 2 == 0);
    }

    void run();
    std::string match_summary_string();

private:
    Player player1;
    Player player2;
    MatchOutcome matchOutcome;
};

}  // namespace hexchess::core

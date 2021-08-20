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
#include <memory>

#include "board.h"
#include "game_outcome.h"
#include "player.h"
#include "util_hexchess.h"


namespace hexchess::core {

template <typename Variant>
class Game {
public:
    typedef Variant V;

    const std::string pgn_string() const;

    Game(std::shared_ptr<Player> wp, std::shared_ptr<Player> bp)
        : _bPlayer{bp}, _wPlayer{wp}
    { }

    const GameOutcome& play();

    std::shared_ptr<Player> getPlayer(Color c);
    // void setPlayer(Color c, std::shared_ptr<Player> player);
    const std::string playerName(Color c) {
        return c == Color::Black ? _bPlayer->name() : _wPlayer->name();
    }

    const std::string game_summary_string() const;

    Board<V> board;
    GameOutcome outcome;

private:
    std::shared_ptr<Player> _bPlayer;
    std::shared_ptr<Player> _wPlayer;

    void _announceGameEnd(const GameOutcome &outcome) const;
    void _printGameStats() const;

    // ---------- Private write methods
    void _initPlayers();
    void _reset();
};

}  // namespace hexchess::core

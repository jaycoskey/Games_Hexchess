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
#include <memory>

#include <gtest/gtest.h>

#include "board.h"
#include "game.h"
#include "game_outcome.h"
#include "player.h"
#include "player_simple_random.h"
#include "util.h"
#include "util_hexchess.h"

using std::cout;
using std::string;

using hexchess::core::Board;
using hexchess::core::Color;
using hexchess::core::Game;
using hexchess::core::GameOutcome;

using hexchess::player::Player;
using hexchess::player::PlayerRandom;


/// As of 2021-08-19, fails with NotImplementedException (Game::play())
TEST(GameTest, GameBasic) {
    bool verbose = false;

    std::shared_ptr<Player> player1 = std::make_shared<PlayerRandom>();
    std::shared_ptr<Player> player2 = std::make_shared<PlayerRandom>();

    Game g{false};
    g.setPlayer1(player1);
    g.setPlayer2(player2);

    // TODO: Re-implement to interact with slot/signal design of Player classes.
    // const GameOutcome& outcome = g.play();
    // assert(outcome == g.outcome);
    // cout << "Game ended after " << g.board.currentCounter() << " moves.\n";
    // cout << g.game_summary_string() << "\n";
    // cout << g.pgn_string() << "\n";
    // cout << g.game_summary_string() << "\n";
}

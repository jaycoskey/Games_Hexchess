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
#include "game_outcome.h"
#include "player.h"
#include "player_random.h"
#include "server.h"
#include "util.h"
#include "util_hexchess.h"

using std::cout;
using std::string;

using hexchess::core::Board;
using hexchess::core::Color;
using hexchess::core::GameOutcome;

using hexchess::player::Player;
using hexchess::player::PlayerRandom;

using hexchess::server::Server;


/// As of 2021-08-19, fails with NotImplementedException
TEST(ServerTest, ServerBasic) {
    bool verbose = false;
    using PR = PlayerRandom;

    PR p1{};
    PR p2{};

    Server server{};
    server.setPlayer1(dynamic_cast<Player*>(&p1));
    server.setPlayer2(dynamic_cast<Player*>(&p2));
    server.initializeBoard(Glinski::fenInitial);

    // TODO: Run complete game. Check outcome.
    // TODO: Run 100 games: AlphaBeta vs Random. Assert that AlphaBeta never loses.
}

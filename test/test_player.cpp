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

#include <gtest/gtest.h>

#include "board.h"
#include "move.h"
#include "player.h"
#include "player_preference.h"
#include "variant.h"

using std::cout;

using hexchess::core::Board;
using hexchess::core::Glinski;
using hexchess::core::Move;
using hexchess::core::PieceType;
using hexchess::core::PlayerAction;

using hexchess::player::rowsAdvanced;
using hexchess::player::PlayerPreference;

/// As of 2021-08-19, fails with NotImplementedException (Board<V>::getLegalMoves(Color)
#ifdef NOTYET
// TODO: Modify Player*'s signal/slot methods to be more testable
TEST(PlayerTest, PlayerPreference_Advancing) {
    bool verbose = false;

    PlayerPreference player{rowsAdvanced};
    PlayerAction nextAction = player.getNextAction(Glinski::fenInitial);

    // Bishops can advance furthest on first move
    ASSERT_EQ(nextAction.getMove().pieceType(), PieceType::Bishop);
}
#endif  // NOTYET

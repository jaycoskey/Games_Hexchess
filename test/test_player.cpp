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

#include "board.h"
#include "move.h"
#include "player.h"
#include "variant.h"

using std::cout;

using hexchess::core::Board;
using hexchess::core::Glinski;
using hexchess::core::Move;
using hexchess::core::PieceType;
using hexchess::core::RandomAdvancingPlayer;


/// As of 2021-08-19, fails with NotImplementedException (Board<V>::getLegalMoves(Color)
void test_player_advancing(bool verbose=false) {
    RandomAdvancingPlayer player{};
    const Move& nextMove = player.getNextMove(Glinski::fenInitial);

    // Bishops can advance furthest on first move
    assert(nextMove.pieceType() == PieceType::Bishop);
}


int main(int argc, char *argv[]) {
    test_player_advancing(true);
}

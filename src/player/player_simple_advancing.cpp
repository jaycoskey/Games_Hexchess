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

#include "board.h"
#include "move.h"
#include "player.h"
#include "player_simple_advancing.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::player {

using core::Board;
using core::Glinski;
using core::Index;
using core::Move;
using core::OptColorPieceType;
using core::PlayerAction;
using core::PlayerActionEnum;

using player::PlayerAdvancing;

Value advancingMoveValuator(const Board<Glinski>& b, const Move& m) {
   Value result = 0;

    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        OptColorPieceType ocpt = b.piecesSparse().at(index);
        if (ocpt.has_value()) {
            auto [c, pt] = ocpt.value();
            if (c == b.mover()) {
                result += Glinski::rowIncreasingForward(index, c);
            }
        }
    }
    return result;
};

// ========================================
// Broadcast
void PlayerAdvancing::initializeBoard(const Fen<V>& fen) {
    _board.initialize(fen);
}

void PlayerAdvancing::showCheckToPlayer(Color checked, Index kingInd) {
    // No action needed; This simple computer player doesn't handle check.
}

// ========================================
// Sent individually
void PlayerAdvancing::showActionRequestToPlayer(Color mover) {
    const Moves& moves = _board.getLegalMoves(_board.mover());

    std::function<Value(const Move&)> valuator
        = [this](const Move& m) -> Value
          { return advancingMoveValuator(this->_board, m); };
    const Moves& bestMoves = maxValues(moves, valuator);
    PlayerAction action{chooseRandom(bestMoves)};
    sendActionToServer(mover, action);
}

void PlayerAdvancing::showActionToPlayer(Color mover, PlayerAction& action) {
    // TODO: Support Draw offers, acceptances, and declines
    if (action.playerActionEnum() == PlayerActionEnum::Move) {
        _board.moveExec(action.move());
    }
}

void PlayerAdvancing::showGameOutcomeToPlayer(
        Color reader, const GameOutcome& gameOutcome)
{
    // No action needed for this computer player
}

}  // namespace hexchess::player

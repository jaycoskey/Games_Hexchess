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
#include "player_simple_attacking.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::player {

using core::Index;
using core::OptColorPieceType;
using core::PlayerAction;
using core::PlayerActionEnum;

// ========================================
// Broadcast
void PlayerAttacking::initializeBoard(const Fen<V>& fen) {
    _board.initialize(fen);
}

void PlayerAttacking::showCheckToPlayer(Color checked, Index kingInd) {
    // No action needed; This simple computer player doesn't handle check.
}

// ========================================
// Sent individually
void PlayerAttacking::showActionRequestToPlayer(Color mover) {
    const Moves& moves = _board.getLegalMoves(_board.mover());
    Moves captures{};
    std::copy_if(
        moves.cbegin(),
        moves.cend(),
        captures.begin(),
        [](const Move& m) { return m.isCapture(); }
        );
    if (captures.size() > 0) {
        PlayerAction action{chooseRandom(captures)};
        sendActionToServer(mover, action);
    } else {
        PlayerAction action{chooseRandom(moves)};
        sendActionToServer(mover, action);
    }
}

void PlayerAttacking::showActionToPlayer(Color mover, PlayerAction& action) {
    // TODO: Support Draw offers, acceptances, and declines
    if (action.playerActionEnum() == PlayerActionEnum::Move) {
        _board.moveExec(action.move());
    }
}

void PlayerAttacking::showGameOutcomeToPlayer(Color reader, const GameOutcome& gameOutcome) {
    // No action needed for this computer player
}

}  // namespace hexchess::player

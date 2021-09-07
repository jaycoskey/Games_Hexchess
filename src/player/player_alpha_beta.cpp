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
#include "player_action.h"
#include "player_alpha_beta.h"
#include "search.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::player {

using core::Board;
using core::Move;
using core::PlayerAction;
using core::PlayerActionEnum;

// ========================================
// Broadcast
void PlayerAlphaBeta::initializeBoard(const Fen<V>& fen) {
    _board.initialize(fen);
}

void PlayerAlphaBeta::showCheckToPlayer(Color checked, Index kingInd) {
    // No action needed; This simple computer player doesn't handle check.
}

// ========================================
// Sent individually
void PlayerAlphaBeta::showActionRequestToPlayer(Color mover) {
    const Moves& moves = _board.getLegalMoves(mover);
    assert(moves.size() > 0);
    const auto& [optBestMove, _] = searchAlphaBeta(_board, mover, _minDepth);
    PlayerAction action{optBestMove.value()};
    sendActionToServer(mover, action);
}

void PlayerAlphaBeta::showActionToPlayer(Color mover, PlayerAction& action) {
    // TODO: Support Draw offers, acceptances, and declines
    if (action.playerActionEnum() == PlayerActionEnum::Move) {
        _board.moveExec(action.move());
    }
}

void PlayerAlphaBeta::showGameOutcomeToPlayer(Color reader,
                                              const GameOutcome& gameOutcome)
{
    // No action needed for this computer player
}

}  // namespace hexchess::player

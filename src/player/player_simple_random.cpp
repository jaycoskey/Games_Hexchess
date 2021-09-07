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

#include <time.h>

#include <iostream>

#include "board.h"
#include "fen.h"
#include "player.h"
#include "player_action.h"
#include "player_simple_random.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"

using std::cout;
using std::string;


namespace hexchess::player {

using core::Board;
using core::Color;
using core::Fen;
using core::Move;
using core::PlayerAction;
using core::PlayerActionEnum;
using core::Glinski;

// ========================================
// Broadcast
void PlayerRandom::initializeBoard(const Fen<V>& fen) {
    if (hexchess::events_verbose) {
        cout << "Player " << _name << " receives board initialization\n";
    }
    _board.initialize(fen);
}

void PlayerRandom::showCheckToPlayer(Color checked, Index kingInd) {
    // No action needed; Computer player will detect check
}

// ========================================
// Sent individually
void PlayerRandom::showActionRequestToPlayer(Color mover) {
    if (hexchess::events_verbose) {
        cout << "============================================================\n";
        cout << "Player " << _name << " (" << mover << ") "
             << "receives action request #"
             << _board.currentCounter() << "\n";
    }
    Moves legalMoves = _board.getLegalMoves(_board.mover());
    Move move = chooseRandom(legalMoves);
    _board.moveExec(move);  // Execute own move
    PlayerAction action{move};
    static timespec nap{0, 10'000'000};
    nanosleep(&nap, NULL);
    if (hexchess::events_verbose) {
        cout << "Player " << _name << " (" << mover << ") "
             << "sending action to server: "
             << action.player_action_string(false) << "\n";
    }
    sendActionToServer(mover, action);
}

void PlayerRandom::showActionToPlayer(Color mover, PlayerAction& action)
{
    if (hexchess::events_verbose) {
        cout << "Player " << _name << " (" << mover << ") "
             << "receives opponent's action: "
             << action.player_action_string(false) << "\n";
    }
    // TODO: Support Draw offers, acceptances, and declines
    if (action.playerActionEnum() == PlayerActionEnum::Move) {
        if (hexchess::events_verbose) {
            cout << "Player " << _name << " (" << mover << ") "
                 << "carrying out opponent move: "
                 << action.move().move_pgn_string(false) << "\n";
        }
        _board.moveExec(action.move());  // Execute opponent's move
    }
}

void PlayerRandom::showGameOutcomeToPlayer(
    Color reader, const GameOutcome& gameOutcome)
{
        // No action needed; Computer player will detect check
}

}  // namespace hexchess::player

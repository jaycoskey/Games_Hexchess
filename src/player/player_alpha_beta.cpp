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
using core::Scope;

// ========================================
// Game <--> Player

// --------------------
// Broadcast
void PlayerAlphaBeta::receiveBoardInitializationFromServer(const Fen<Glinski>& fen) {
    Scope scope{"PlayerAlphaBeta::receiveBoardInitializationFromServer"};

    if (hexchess::events_verbose) {
        print(cout, scope(), "Player ", _name,
            " (counter=", _board.currentCounter(), ")",
            " receives board initialization\n");
    }
    _board.initialize(fen);
    if (_gui) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "Player ", _name,
                " (counter=", _board.currentCounter(), ")",
                " sending board initialization to GUI\n");
        }
        sendBoardInitializationToGui(fen);
    }
}

void PlayerAlphaBeta::receiveCheckFromServer(Color checked, Index kingInd) {
    Scope scope{"PlayerAlphaBeta::receiveCheckFromServer"};

    // No local action needed; Computer player handles checks.
    sendCheckToGui(checked, kingInd);
}

// --------------------
// Sent individually

void PlayerAlphaBeta::receiveActionRequestFromServer(Color mover, const Moves legalMoves) {
    Scope scope{"PlayerAlphaBeta::receiveActionRequestFromServer"};

    if (hexchess::events_verbose) {
        cout << "============================================================\n";
        print(cout, scope(), "Player ", _name, " (mover=", color_long_string(mover),
            ", counter=", _board.currentCounter(), ") ",
            "receives action request #", _board.currentCounter() + 1,
            " (", _board.pieceCount(), " pieces on board)....\n");
    }
    const Moves& moves = _board.getLegalMoves(mover);
    assert(moves.size() > 0);
    auto [optBestMove, _] = searchAlphaBeta(_board, mover, _minDepth);
    assert(optBestMove.has_value());
    _board.moveExec(optBestMove.value());
    (void) _board.getLegalMoves(_board.mover());
    (void) _board.getCheckEnum();

    PlayerAction action{optBestMove.value()};
    // No need to ask human for move
    // if (_gui) {
    //     sendActionRequestToGui(mover, legalMoves);
    // }
    if (hexchess::events_verbose) {
        print(cout, scope(), "Player ", _name, " (", color_long_string(mover),
            ", counter=", _board.currentCounter(), ")",
            ", sending action by ", color_long_string(action.move().mover()),
            " to server: ", action.player_action_string(false), "\n");
    }
    sendActionToServer(mover, action);

    if (_gui) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "Player ", _name, " (", color_long_string(mover),
                ", counter=", _board.currentCounter(), ")",
                ", sending action by ", color_long_string(action.move().mover()),
                " to GUI: ", action.player_action_string(false), "\n");
        }
        sendActionToGui(mover, action);
    }
}

void PlayerAlphaBeta::receiveActionFromServer(Color mover, const PlayerAction action) {
    Scope scope{"PlayerAlphaBeta::receiveActionFromServer"};

    if (hexchess::events_verbose) {
        print(cout, scope(), "Player ", _name, " (", mover,
            ", counter=", _board.currentCounter(), ") ",
            "receives opponent's action: ", action.player_action_string(false), "\n");
    }
    // TODO: Support Draw offers, acceptances, and declines
    if (action.playerActionEnum() == PlayerActionEnum::Move) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "Player ", _name, " (", mover,
                ", counter=", _board.currentCounter(), ") ",
                 "carrying out opponent move: ",
                 action.move().move_pgn_string(false), "\n");
        }
        _board.moveExec(action.move());
        (void) _board.getLegalMoves(_board.mover());
        (void) _board.getCheckEnum();
    }
    if (_gui) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "Player ", _name, " (mover=", color_long_string(mover),
                ", counter=", _board.currentCounter(), ") ",
                " sending action to GUI\n");
        }
        sendActionToGui(mover, action);
    }
}

void PlayerAlphaBeta::receiveGameOutcomeFromServer(
    Color receiver, const GameOutcome& gameOutcome)
{
    Scope scope{"PlayerAlphaBeta::receiveGameOutcomeFromServer"};

    // No action needed for this computer player
    if (_gui) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "Player ", _name, " (received by ", color_long_string(receiver),
                ", counter=", _board.currentCounter(), ") ",
                " sending game outcome to GUI\n");
        }
        sendGameOutcomeToGui(receiver, gameOutcome);
    }
}

// ========================================
// Player <--> GUI

void PlayerAlphaBeta::receiveActionFromGui(Color mover, const PlayerAction& action) {
    // This is a computer player; it should not receive actions from the GUI.
    throw std::logic_error{"PlayerAlphaBeta, a computer player, should not receive player actions from the GUI"};
}

}  // namespace hexchess::player

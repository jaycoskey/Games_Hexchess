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
#include <sstream>

#include "board.h"
#include "fen.h"
#include "player.h"
#include "player_action.h"
#include "player_random.h"
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
using core::Scope;
using core::Size;

// ========================================
// Game <--> Player

// --------------------
// Broadcast
void PlayerRandom::receiveBoardInitializationFromServer(const Fen<V>& fen) {
    Scope scope{"PlayerRandom::receiveBoardInitializationFromServer"};

    if (hexchess::events_verbose) {
        print(cout, scope(), "PlayerRandom=", name(), ", counter=", _board.currentCounter(),
            ". Player receives board initialization\n");
    }
    _board.initialize(fen);
    if (_gui) { sendBoardInitializationToGui(fen); }
}

void PlayerRandom::receiveCheckFromServer(Color checked, Index kingInd) {
    // No action needed; Computer player will detect check
    if (_gui) { sendCheckToGui(checked, kingInd); }
}

// --------------------
// Sent individually

void PlayerRandom::receiveActionRequestFromServer(Color mover, const Moves legalMoves) {
    Scope scope{"PlayerRandom::receiveActionRequestFromServer"};

    if (hexchess::events_verbose) {
        cout << "============================================================\n";
        print(cout, scope(), "PlayerRandom=", name(), ", counter=", _board.currentCounter(),
            ", mover=", color_long_string(mover),
            ". Player receives action request #", _board.currentCounter() + 1,
            " (", _board.pieceCount(), " pieces on board).\n\tBoard:\n", _board.board_string());
    }
    assert(_gui);
    bool verbose = true;

    if (verbose) {
        std::ostringstream oss;
        for (Size k = 0; k < legalMoves.size(); ++k) {
            const Move& move = legalMoves.at(k);
            if (_board.isPieceAt(move.to(), mover)) {
                print(oss, "\t", scope(), "PlayerRandom=", name(), "Counter=", _board.currentCounter(),
                    ". Warning: Legal move #", k + 1,
                    " is ", move.move_pgn_string(false),
                    ", but there is a ", move.mover(), " piece at ", move.to(),
                    " ( ", _board.getPieceTypeAt(move.to(), mover), ")\n");
            }
        }
        print(cout, scope(), "PlayerRandom=", name(), ", counter=", _board.currentCounter(),
            ": ", oss.str());
    }
    Move move = chooseRandom(legalMoves);
    if (verbose) {
        assert(move.from() != move.to());
        assert(move.from() >= 0 && move.from() < Glinski::CELL_COUNT);
        assert(move.to() >= 0 && move.to() < Glinski::CELL_COUNT);
        if (_board.getPieceTypeAt(move.from(), mover) != move.pieceType())
        {
            print(cout, scope(), "PlayerRandom=", name(), ", counter=", _board.currentCounter(),
                ". Assertion failing:\n",
                "\tmove.from()=", move.from(), "\n",
                "\tmove.pieceType()=", move.pieceType(), "\n",
                "\tmover=", mover, "\n",
                "\t_board.getPieceTypeAt(move.from(), mover)=",
                    _board.getPieceTypeAt(move.from(), mover), "\n");

            assert(_board.getPieceTypeAt(move.from(), mover) == move.pieceType());
        }
        assert(move.isCapture() || !_board.isPieceAt(move.to()));
    }
    if (verbose) {
        print(cout, scope(), "PlayerRandom=", name(), ", counter=", _board.currentCounter(),
            ", mover=", color_long_string(mover), " ********** ",
            "Calling Board::moveExec() with move=", move.move_pgn_string(false),
            "\n");
    }
    _board.moveExec(move);  // Execute own move

    PlayerAction action{move};
    if (_gui) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "PlayerRandom=", name(), ", counter=", _board.currentCounter(),
            ", action=", action.player_action_string(false),
            ", sending action to GUI: ", action.player_action_string(false), "\n");
        }
        sendActionToGui(mover, action);
    }
    if (hexchess::events_verbose) {
        print(cout, scope("Event: "),
            "PlayerRandom=", name(), ", counter=", _board.currentCounter(),
            ", mover=", color_long_string(mover),
            ". Sending action by ", color_long_string(action.move().mover()),
            " to server: ", action.player_action_string(false), "\n");
    }
    sendActionToServer(mover, action);
}

void PlayerRandom::receiveActionFromServer(Color mover, const PlayerAction action) {
    Scope scope{"PlayerRandom:receiveActionFromServer"};

    if (hexchess::events_verbose) {
        print(cout, scope("Event: "),
            "PlayerRandom=", name(), ", counter=", _board.currentCounter(),
            ", mover=", mover,
            "receives opponent's action: ", action.player_action_string(false), "\n");
    }
    // TODO: Support Draw offers, acceptances, and declines
    if (action.playerActionEnum() == PlayerActionEnum::Move) {
        if (hexchess::events_verbose) {
            print(cout, scope("Event: "),
                "PlayerRandom=", name(), ", counter=", _board.currentCounter(),
                ", mover=", mover,
                "carrying out opponent move: ", action.move().move_pgn_string(false), "\n");
        }
        _board.moveExec(action.move());  // Execute opponent's move
        (void) _board.getLegalMoves(_board.mover());
        (void) _board.getCheckEnum();
    }
    if (_gui) { sendActionToGui(mover, action); }
}

void PlayerRandom::receiveGameOutcomeFromServer(
    Color receiver, const GameOutcome& gameOutcome)
{
    // No action needed; Computer player will detect check
    if (_gui) { sendGameOutcomeToGui(receiver, gameOutcome); }
}

// ========================================

void PlayerRandom::receiveActionFromGui(Color mover, const PlayerAction& action) {
    // This is a computer player; it should not receive actions from the GUI.
    throw std::logic_error{"PlayerRandom, a computer player, should not receive player actions from the GUI"};
}

}  // namespace hexchess::player

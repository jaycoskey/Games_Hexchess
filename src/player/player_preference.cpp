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
#include "player_preference.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::player {

using core::Index;
using core::OptColorPieceType;
using core::PlayerAction;
using core::PlayerActionEnum;

// ========================================
// Game <--> Player

// --------------------
// Broadcast
void PlayerPreference::receiveBoardInitializationFromServer(const Fen<Glinski>& fen) {
    _board.initialize(fen);
    if (_gui) { sendBoardInitializationToGui(fen); }
}

void PlayerPreference::receiveCheckFromServer(Color checked, Index kingInd) {
    // No local action needed; Computer player handles checks.
    if (_gui) { sendCheckToGui(checked, kingInd); }
}

// --------------------
// Sent individually

// Computer player---ActionRequest not forwarded to GUI
void PlayerPreference::receiveActionRequestFromServer(Color mover, const Moves legalMoves) {
    Moves preferred{};
    if (_mode == PreferenceMode::Maximize) {
        std::function<Value(const Move&)> valuator
            = [this](const Move& m) -> Value
                { return _maxFunc(m); };
        const Moves& bestMoves = maxValues(legalMoves, valuator);
        PlayerAction action{chooseRandom(bestMoves)};
        sendActionToServer(mover, action);
    } else if (_mode == PreferenceMode::Predicate) {
        std::copy_if(legalMoves.cbegin(), legalMoves.cend(), preferred.begin(), _predFunc);
        if (preferred.size() > 0) {
            PlayerAction action{chooseRandom(preferred)};
            sendActionToServer(mover, action);
            if (_gui) { sendActionToGui(mover, action); }
        } else {
            PlayerAction action{chooseRandom(legalMoves)};
            sendActionToServer(mover, action);
            if (_gui) { sendActionToGui(mover, action); }
        }
    } else {
        throw std::logic_error{"PlayerPreference::receiveActionRequestFromServer: Unrecognized preference mode"};
    }
}

void PlayerPreference::receiveActionFromServer(Color mover, const PlayerAction action) {
    // TODO: Support Draw offers, acceptances, and declines
    if (action.playerActionEnum() == PlayerActionEnum::Move) {
        _board.moveExec(action.move());
        (void) _board.getLegalMoves(_board.mover());
        (void) _board.getCheckEnum();
    }
    if (_gui) { sendActionToGui(mover, action); }
}

void PlayerPreference::receiveGameOutcomeFromServer(Color receiver, const GameOutcome& gameOutcome) {
    // No action needed for this computer player
    if (_gui) { sendGameOutcomeToGui(receiver, gameOutcome); }
}

// ========================================
// Player <--> GUI

void PlayerPreference::receiveActionFromGui(Color mover, const PlayerAction& action) {
    // This is a computer player; it should not receive actions from the GUI.
    throw std::logic_error{"PlayerPreference, a computer player, should not receive player actions from the GUI"};
}

bool isPieceCapture(const Move& move) {
    return move.isCapture();
}

Short rowsAdvanced(const Move& move) {
    Short fromRow = Glinski::rowIncreasingForward(move.from(), move.mover());
    Short toRow = Glinski::rowIncreasingForward(move.to(), move.mover());
    return toRow - fromRow;
}

}  // namespace hexchess::player

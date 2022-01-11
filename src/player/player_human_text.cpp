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

#include <cctype>

#include <functional>
#include <iostream>
#include <string>

#include "player.h"
#include "player_action.h"
#include "player_human_text.h"
#include "util_hexchess.h"

using std::cin;
using std::cout;
using std::string;


namespace hexchess::player {

using core::Move;
using core::PlayerAction;
using core::PlayerActionEnum;
using core::Size;
using core::Strings;

string input(const string& prompt,
             std::function<bool(string)> isValid=[](string s){return true;},
             std::function<string(string)> xform=[](string s){return s;}
            )
{
    bool isInputValid = false;

    while (!isInputValid) {
        cout << prompt;
        string response;
        getline(cin, response);
        if (isValid(response)) {
            return xform(response);
        }
        continue;
    }
    return "";  // Not reached
}

const string PlayerHumanText::_helpMessage =
    "Available player actions:\n"
    "\t<move>, such as \"b1 b3\" (lower or upper case)\n"
    // Note: No  need to mention Castling here, since it is not part of Glinski chess

    "\t\tOffer a draw to your opponent\n"
    "\tdraw\n"
    "\t\tOffer a draw to your opponent\n"

    "\tresign\n"
    "\t\tResign from the game.\n"

    "\tmoves\n"
    "\t\tList legal moves.\n"

    "\thelp\n"
    "\t\tPrints this help message.\n";

void PlayerHumanText::receiveBoardInitializationFromServer(const Fen<V>& fen) {
    Scope scope{"PlayerHumanText::receiveBoardInitializationFromServer"};

    _board.initialize(fen);
    if (_gui) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "PlayerHumanText::receiveBoardInitializationFromServer: Sending board initialization to MainWindow\n");
        }
        sendBoardInitializationToGui(fen);
    }
}
void PlayerHumanText::receiveCheckFromServer(Color checked, Index kingInd) {
    Scope scope{"PlayerHumanText::receiveCheckFromServer"};

    cout << color_long_string(checked) << " is in check\n";
    if (_gui) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "PlayerHumanText::receiveBoardInitializationFromServer: Sending Check to MainWindow\n");
        }
        sendCheckToGui(checked, kingInd);
    }
}

void PlayerHumanText::receiveGameOutcomeFromServer(Color receiver, const GameOutcome& gameOutcome) {
    Scope scope{"PlayerHumanText::receiveGameOutcomeFromServer"};

    cout << gameOutcome.game_outcome_reader_string(receiver) << "\n";
    if (_gui) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "PlayerHumanText::receiveBoardInitializationFromServer: Sending GameOutcome to MainWindow\n");
        }
        sendGameOutcomeToGui(receiver, gameOutcome);
    }
}

// Note: ActionRequest not forwarded to GUI for this text-based player
void PlayerHumanText::receiveActionRequestFromServer(Color mover, const Moves legalMoves) {
    Scope scope{"PlayerHumanText::receiveActionRequestFromServer"};
    print(cout, scope(), "[mover=", mover, "]: Board:\n", _board.board_string());

    while (true) {
        cout << "Enter move (e.g., \"b1 b3\"), \"draw\", \"resign\", \"moves\", or \"help\": ";
        string input;
        getline(cin, input);
        for (Size k = 0; k < input.size(); ++k) {
            input[k] = toupper(input[k]);
        }

        // TODO: Remove blank spaces, and convert to upper case
        if (input == "HELP") {
            cout << _helpMessage;
        } else if (input == "DRAW") {
            PlayerAction action{PlayerActionEnum::Draw_Offer};
            sendActionToServer(mover, action);
            break;
        } else if (input == "RESIGN") {
            PlayerAction action{PlayerActionEnum::Resign};
            sendActionToServer(mover, action);
            break;
        } else if (input == "MOVES") {
            cout << "Legal moves: ";
            for (const Move& move : legalMoves) {
                cout << move.move_pgn_string(false) << ", ";
            }
            cout << "\n";
            continue;
        } else {
            Strings parts = split(' ', input);
            if (parts.size() != 2) {
                cout << "Invalid move: Must consist of two parts: initial cell, and destination cell.\n";
                cout << "Please try again.\n";
                continue;
            }

            Index from;
            try {
                from = Glinski::cellNameToIndex(parts[0]);
            }
            catch (std::out_of_range& e) {
                cout << "First entry " << parts[0] << " is not a valid cell.\n";
                cout << "Please try again.\n";
                continue;
            }

            Index to;
            try {
                to = Glinski::cellNameToIndex(parts[1]);
            }
            catch (std::out_of_range& e) {
                cout << "First entry " << parts[1] << " is not a valid cell.\n";
                cout << "Please try again.\n";
                continue;
            }

            bool isValidMove = false;
            for (const Move& move : legalMoves) {
                if (from == move.from() && to == move.to()) {
                    PlayerAction action{move};
                    sendActionToServer(mover, action);
                    break;
                }
            }
            cout << "Moving " << parts[0] << " to " << parts[1] << " is not a legal move.\n";
            cout << "Please try again.\n";
        }
    }
}

void PlayerHumanText::receiveActionFromServer(Color mover, const PlayerAction action) {
    Scope scope{"PlayerHumanText::receiveActionFromServer"};

    switch(action.playerActionEnum()) {
    case PlayerActionEnum::Move:
        print(cout, scope(), ", counter=", _board.currentCounter(), ". Board:\n", _board.board_string());
        print(cout, scope(), ", counter=", _board.currentCounter(), ". Calling moveExec\n");
        _board.moveExec(action.move());
        print(cout, scope(), ", counter=", _board.currentCounter(), ". Caching legal moves\n");
        (void) _board.getLegalMoves(_board.mover());
        print(cout, scope(), ", counter=", _board.currentCounter(), ". getting CheckEnum\n");
        (void) _board.getCheckEnum();
        break;
    case PlayerActionEnum::Draw_Offer:
        throw NotImplementedException{"PlayerHumanText::receiveActionFromServer - Draw_Offer"};
        break;
    case PlayerActionEnum::Draw_Accept:
        throw NotImplementedException{"PlayerHumanText::receiveActionFromServer - Draw_Accept"};
        // TODO: Implement
        break;
    case PlayerActionEnum::Draw_Decline:
        throw NotImplementedException{"PlayerHumanText::receiveActionFromServer - Draw_Decline"};
        // TODO: Implement
        break;
    case PlayerActionEnum::Resign:
        cout << "Your opponent resigned\n";
        break;
    }
    if (_gui) { sendActionToGui(mover, action); }
}

void PlayerHumanText::receiveActionFromGui(Color mover, const PlayerAction& action) {
    // This is a human text player; it should not receive actions from the GUI.
    throw std::logic_error{"PlayerAlphaBeta, a computer player, should not receive player actions from the GUI"};
}

// ****************************************

}  // namespace hexchess::player

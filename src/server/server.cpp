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

#include <cassert>
#include <cctype>
#include <time.h>

#include <iostream>
#include <memory>
#include <regex>
#include <stdexcept>

#include <Qt>
#include <QTest>

#include "game_outcome.h"
#include "move.h"
#include "server.h"
#include "util_hexchess.h"

using std::cout;
using std::string;

using std::vector;

using QTest::qWait;


namespace hexchess::server {

using hexchess::split;

using hexchess::core::CheckEnum;
using hexchess::core::MoveEnum;
using hexchess::core::OptPieceType;
using hexchess::core::PieceType;
using hexchess::core::Scope;
using hexchess::core::Size;
using hexchess::core::Termination;
using hexchess::core::piece_type_parse;

static int sleepTimeMs = 100;

Server::Server()
    : board{"Server-Board", false},
      outcome{Termination::None},
      player1{NULL},
      player2{NULL},
      _event{"Hexagonal Chess"},
      _site{"Virtual"},
      _round{"1"},
      _variant{"Glinski"}
{
    const Scope scope{"Server::Server"};
    bool verbose = true;

    // Set date and time strings
    time_t unixTime = time(&unixTime);
    struct tm* timeInfo = gmtime(&unixTime);
    char buffer[100];

    // Note: Could instead use std::put_time (since C++11)
    strftime(buffer, sizeof(buffer), "%Y.%m.%d", timeInfo);
    string date_{buffer, buffer + 10};
    if (verbose) { print(cout, scope(), "Current date = <<", date_, ">>\n"); }

    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeInfo);
    string time_{buffer, buffer + 8};
    if (verbose) { print(cout, scope(), "Current time (UTC) = <<", time_, ">>\n"); }
    // \todo Use current date and time in saving games in PGN format
}

Server::Server(const Server& other)
    : board{other.board},
      outcome{other.outcome},
      player1{other.player1},
      player2{other.player2},
      _event{other._event},
      _site{other._site},
      _round{other._round},
      _variant{other._variant},
      _date{other._date},
      _time{other._time},
      _otherTags{other._otherTags}
{ }

const GameOutcome& Server::play() {
    const Scope scope{"main.cpp:play"};
    while (true) {
        // app.processEvents();
        Color mover = board.mover();
        if (mover == Color::Black) {
            if (hexchess::events_verbose) {
                print(cout, scope(), "Counter=", board.currentCounter(),
                    ". Calling Server::sendActionRequestToPlayer2\n");
            }
            sendActionRequestToPlayer2(mover, board.getLegalMoves(mover));
        } else {
            if (hexchess::events_verbose) {
                print(cout, scope(), "Counter=", board.currentCounter(),
                    ". Calling Server::sendActionRequestToPlayer1\n");
            }
            sendActionRequestToPlayer1(mover, board.getLegalMoves(mover));
        }
        // app.processEvents();
    }
}

Player* Server::getPlayer(Color c) {
    return c == Color::Black ? player2 : player1;
}

void Server::setPlayer1(Player *p1) {
    const Scope scope{"Server::setPlayer1"};
    print(cout, scope(), "Counter=", board.currentCounter(),
        ". Setting Player 1 to ", p1->name(), "\n");
    player1 = p1;
}
void Server::setPlayer2(Player *p2) {
    const Scope scope{"Server::setPlayer2"};
    print(cout, scope(), "Counter=", board.currentCounter(),
        ". Setting Player 2 to ", p2->name(), "\n");
    player2 = p2;
}

void Server::initializeBoard(const Fen<Glinski>& fen) {
    const Scope scope{"Server::initializeBoard"};
    print(cout, scope(), "Counter=", board.currentCounter(),
        ". Initializing the board\n");
    board.initialize(fen);
    if (hexchess::events_verbose) {
        print(cout, scope("Event: "), "Counter=", board.currentCounter(),
            ". Sending board initialization to players\n");
    }
    // qWait(sleepTimeMs);
    sendBoardInitializationToPlayers(fen);
}

void Server::initializeBoard(const std::string& fenStr) {
    initializeBoard(Fen<Glinski>{fenStr});
}

const std::string Server::playerName(Color c) const {
    return c == Color::Black ? player2->name() : player1->name();
}

void Server::receiveActionFromPlayer(Color mover, PlayerAction action) {
    Scope scope{"Server::receiveActionFromPlayer"};
    assert(mover == action.move().mover());
    bool verbose = true;
    bool debug = true;

    if (hexchess::events_verbose) {
        print(cout, scope("Event: "), "Counter=", board.currentCounter(),
            ". Server receives action by ", color_long_string(mover), ": ",
            action.player_action_string(false), "\n");
    }

    switch(action.playerActionEnum()) {
    case PlayerActionEnum::Move: {
        if (verbose) {
            print(cout, scope(), "Counter=", board.currentCounter(),
                ". Action is a Move. Is it one of the ",
                board.getLegalMoves(mover).size(), " legal moves?\n");
        }
        // \todo Speed up with hash (or multi-level map: PieceType->Moves)
        bool isMoveLegal = false;
        for (const Move& legalMove : board.getLegalMoves(mover)) {
            if (legalMove != action.move()) {
                continue;
            }
            isMoveLegal = true;
            if (verbose) {
                print(cout, scope(), "Counter=", board.currentCounter(),
                    ". Yes: Move is legal. Calling Board::moveExec() to execute ",
                    action.player_action_string(false),
                    " as move #", board.currentCounter() + 1, "\n");
            }
            board.moveExec(legalMove);
            Color nextMover = board.mover();
            assert(nextMover == opponent(mover));
            print(cout, scope(), "Counter=", board.currentCounter(),
                ". Server getting legal moves for next player: ",
                color_long_string(board.mover()), "\n");
            (void) board.getLegalMoves(board.mover());
            print(cout, scope(), "Counter=", board.currentCounter(),
                ". Server getting checkEnum for next player: ",
                color_long_string(board.mover()), "\n");
            (void) board.getCheckEnum();

            // Inform opponent of Player's move
            if (nextMover == Color::Black) {
                if (hexchess::events_verbose) {
                    print(cout, scope("Event: "), "Counter=", board.currentCounter(),
                        ". Server sending player #1's action to player #2. Action=",
                        action.player_action_string(false), "\n");
                }
                sendActionToPlayer2(mover, action);
                print(cout, scope("Event: "), "Counter=", board.currentCounter(),
                    ". Server sent player #1's action to player #2\n");
            } else {
                if (hexchess::events_verbose) {
                    print(cout, scope("Event: "), "Counter=", board.currentCounter(),
                        "Server sending player #2's action to player #1. Action=",
                        action.player_action_string(false), "\n");
                }
                sendActionToPlayer1(mover, action);
                print(cout, scope("Event: "), "Counter=", board.currentCounter(),
                    "Server sent player #2's action to player #1\n");
            }
            print(cout, scope(), "Counter=", board.currentCounter(), ". Testing check\n");
            if (board.isCheck()) {
                if (hexchess::events_verbose) {
                    print(cout, scope("Event: "), "Counter=", board.currentCounter(),
                        ". Server sending Check to players");
                }
                sendCheckToPlayers(mover, board.getKingIndex(mover));
            }

            print(cout, scope(), "Counter=", board.currentCounter(), ". Testing Outcome\n");
            if (board.getOptOutcome().has_value()
                && board.getOptOutcome().value().termination() != Termination::None)
            {
                GameOutcome outcome = board.getOptOutcome().value();

                if (hexchess::events_verbose) {
                    print(cout, scope("Event: "), "Counter=", board.currentCounter(),
                        ". Server sending game outcome (",
                        outcome.game_outcome_short_string(), ") to player #1\n");
                }
                sendGameOutcomeToPlayer1(Color::White, outcome);

                if (hexchess::events_verbose) {
                    print(cout, scope("Event: "), "Counter=", board.currentCounter(),
                        ". Server sending game outcome (",
                        outcome.game_outcome_short_string(), ") to player #1\n");
                }
                sendGameOutcomeToPlayer2(Color::Black, outcome);
                if (hexchess::events_verbose) {
                    print(cout, scope("Event: "), "Counter=", board.currentCounter(),
                        ". Sent action request to player #2\n");
                }
            }

            // qWait(sleepTimeMs);
            if (nextMover == Color::Black) {
                if (hexchess::events_verbose) {
                    print(cout, scope(), "Counter=", board.currentCounter(),
                        ". Calling Server::sendActionRequestToPlayer2\n");
                }
                sendActionRequestToPlayer2(nextMover, board.getLegalMoves(nextMover));
                if (hexchess::events_verbose) {
                    print(cout, scope("Event: "), "Counter=", board.currentCounter(),
                        ". Sent action request to player #2\n");
                }
            } else {
                if (hexchess::events_verbose) {
                    print(cout, scope(), "Counter=", board.currentCounter(),
                        ". Calling Server::sendActionRequestToPlayer1\n");
                }
                sendActionRequestToPlayer1(nextMover, board.getLegalMoves(nextMover));
                if (hexchess::events_verbose) {
                    print(cout, scope("Event: "), "Counter=", board.currentCounter(),
                        ". Sent action request to player #1\n");
                }
            }
        }
        if (!isMoveLegal) {
            throw std::logic_error{"Server::receiveActionFromPlayer: Move is illegal"};
        }
        } // case PlayerActionEnum::Move
        break;
    case PlayerActionEnum::Draw_Offer:
        throw new NotImplementedException("Server:receiveActionFromPlayer: action=Draw_Offer");
        break;
    case PlayerActionEnum::Draw_Accept:
        throw new NotImplementedException("Server:receiveActionFromPlayer: action=Draw_Accept");
        break;
    case PlayerActionEnum::Draw_Decline:
        throw new NotImplementedException("Server:receiveActionFromPlayer: action=Draw_Decline");
        break;
    case PlayerActionEnum::Resign:
        throw new NotImplementedException("Server:receiveActionFromPlayer: action=Draw_Resign");
        // TODO: Implement receivePlayerActionFromPlayer: Resign
        break;
    default:
        throw std::logic_error("receivePlayerActionFromPlayer: Unrecognized player action");
    }
}

const string Server::game_summary_string() const {
    std::ostringstream oss;

    oss << "Players=("
        <<   "White:\"" <<  player1->name() << "\", "
        <<   "Black:\"" <<  player2->name() << "\", "
        <<   "), "
        << "Counter=" << board.currentCounter() << ". Outcome="
        ;

    if (outcome.isWin()) {
        oss << "Win for " << color_long_string(outcome.winner())
            << " by " << termination_string(outcome.termination());
    } else if (outcome.termination() == Termination::Draw_Stalemate) {
        oss << "Stalemate by " << outcome.winner();
    } else /* Other form of Draw */ {
        oss << "Draw (" << termination_string(outcome.termination()) << ")";
    }
    oss << ", ";
    oss << "Score(W)=" << outcome.score(Color::White) << ", ";
    oss << "Score(B)=" << outcome.score(Color::Black) << ".";

    return oss.str();
}

/// \todo Handle more than one game in the pgn input string
/// \todo Make parsing more robust (BNF/PEG/Pratt/etc.?)
/// \todo Handle PGN comment syntax
void Server::load_pgn(const string& pgn) {
    const Scope scope{"Server::load_pgn"};
    assert(board.currentCounter() == 0);
    assert(pgn.size() > 0);

    std::stringstream ss{pgn};
    std::string line;
    bool foundMoves = false;;

    print(cout, scope("Board: "), "\n");
    print(cout, scope(), board.board_string());

    // \todo Make handling of line endings cross-platform
    while (std::getline(ss, line, '\n')) {
        print(cout, scope(), ": parsing line=", line, "\n");
        if (line[0] == '#') {
            continue;
        }
        if (line[0] == '[') {
            _pgn_tag_parse(line);
        } else if (line[0] == '1') {
            if (!foundMoves) {
                string fenStr = _otherTags.contains("Fen")
                                    ? _otherTags["Fen"]
                                    : Glinski::fenInitial;
                Fen<Glinski> fen{fenStr};
                print(cout, scope(), "Initializating Board & sending message to players\n");
                initializeBoard(fen);
                foundMoves = true;
            }
            for (const std::string& moveStr : split(' ', line)) {
                if (isdigit(moveStr[0])) {
                    continue;
                }
                std::smatch moveMatch;
                string moveRegexStr{
                    "([a-zA-Z]\{1,2\}[0-9]\{1,2\})"  // fromStr
                    "([x-]?)"                        // infix
                    "([a-zA-Z][0-9]\{1,2\})"         // toStr
                    "(.*)"                           // extras
                    };
                std::regex moveRegex{moveRegexStr, std::regex::extended};
                if (!std::regex_search(moveStr, moveMatch, moveRegex)) {
                    print(cout, scope(), "Failed to match ", moveStr,
                        " against ", moveRegexStr, "\n");
                    throw std::invalid_argument(string{"Server::load_pgn: "} + moveStr);
                }
                string fromStr = moveMatch[1];
                string infix   = moveMatch[2];
                string toStr   = moveMatch[3];
                string extras  = moveMatch.size() == 5 ? moveMatch[4] : string{""};

                fromStr[0] = toupper(fromStr[0]);
                toStr[0] = toupper(toStr[0]);

                PieceType pt = PieceType::King;
                if (isalpha(fromStr[0]) && isalpha(fromStr[1])) {
                    pt = piece_type_parse(fromStr[0]);
                    fromStr = fromStr.substr(1,1);
                    fromStr[0] = toupper(fromStr[0]);
                } else {
                    Index fromInd = Glinski::cellNameToIndex(fromStr);
                    pt = board.getPieceTypeAt(fromInd);
                }
                Index from = Glinski::cellNameToIndex(fromStr);
                Index to = Glinski::cellNameToIndex(toStr);
                bool isCapture = board.isPieceAt(to);
                if (infix.size() > 0 && infix[0] == 'x') {
                    assert(isCapture);
                }
                bool isCheck{false};
                bool isCheckmate{false};
                bool isEnPassant{false};
                MoveEnum moveEnum = MoveEnum::Simple;
                OptPieceType optCaptured;
                if (isCapture) {
                    PieceType oppPt = board.getPieceTypeAt(to);
                    optCaptured = std::make_optional<PieceType>(oppPt);
                }
                OptPieceType optPromotedTo;

                for (Size k = 0; k < extras.size(); ++k) {
                    switch(extras[k]) {
                    case '#':
                        isCheckmate = true;
                        break;
                    case '+':
                        isCheck = true;
                        break;
                    case '=':
                        assert(k + 1 < extras.size());
                        optPromotedTo = piece_type_parse((extras[k+1]));
                        break;
                    case 'e':
                        isEnPassant = true;
                        if (k+1 < extras.size() && extras[k+1] == 'p') {
                            k += 2;
                        }
                        if (k+3 < extras.size() && extras.substr(k, 4) == "e.p.") {
                            k += 4;
                        }
                        break;
                    case 'O':
                    case '0':
                        if (k+4 < extras.size()
                            && ((extras.substr(k, 5) == "O-O-O")
                                || (extras.substr(k, 5) == "0-0-0")))
                        {
                            // TODO: Support castling
                            break;
                        }
                        if (k+2 < extras.size()
                            && ((extras.substr(k, 3) == "O-O")
                                || (extras.substr(k, 3) == "0-0")))
                        {
                            // TODO: Support castling
                            break;
                        }
                        break;
                    default:
                        break;
                    }
                }

                /// TODO: Support castling
                if (optPromotedTo != std::nullopt) {
                    moveEnum = MoveEnum::PawnPromotion;
                } else if (pt == PieceType::Pawn
                    && optCaptured != std::nullopt
                    && !board.isPieceAt(to))
                {
                    moveEnum = MoveEnum::EnPassant;
                }

                Move move{
                    board.mover(), pt, from, to,
                    moveEnum, optCaptured, optPromotedTo,
                    isCheck ? CheckEnum::Check
                            : (isCheckmate ? CheckEnum::Checkmate : CheckEnum::None)
                    };
                cout << scope("INFO: ") << "Calling Board::moveExec(): "
                         << "(mover=" << color_long_string(move.mover())
                         << ") calling moveExec for move: "
                         << move.move_pgn_string(false) << "\n";
                board.moveExec(move);
                PlayerAction action{move};
                if (player1) { sendActionToPlayer1(board.mover(), action); }
                if (player2) { sendActionToPlayer2(board.mover(), action); }
            }
        }
    }
}

const string Server::game_pgn_string() const {
    std::ostringstream oss;

    oss << "[Event "   << _event << "]\n";
    oss << "[Site "    << _site << "]\n";
    oss << "[Round "   << _round << "]\n";

    oss << "[Variant " << _variant << "]\n";
    oss << "[Date "    << _date << "]\n";
    // oss << "[Time "    << _time << "]\n";
    oss << "[White "   << playerName(Color::White) << "]\n";
    oss << "[Black "   << playerName(Color::Black) << "]\n";
    if (outcome.termination() != Termination::None) {
        oss << "[Result " << outcome.game_outcome_score_string() << "]\n";
    }
    oss << board.moves_pgn_string();

    return oss.str();
}

/// \todo Add param to determine protocol for syntax errors in tags
void Server::_pgn_tag_parse(const string& line) {
    Size lBracket = line.find("[");
    if (lBracket == std::string::npos) {
        throw std::invalid_argument(string{"Invalid PGN tag line: "} + line);
    }
    Size rBracket = line.find("]");
    if (lBracket == std::string::npos) {
        throw std::invalid_argument(string{"Invalid PGN tag line: "} + line);
    }
    Size space1 = line.find(" ");
    string tag = line.substr(1, space1 - 1);

    Size lQuote = space1 + 1 + line.substr(space1 + 1, rBracket - space1 - 1).find("\"");
    Size rQuote = lQuote + 1 + line.substr(lQuote + 1, rBracket - lQuote - 1).find("\"");
    string value = line.substr(lQuote + 1, rQuote - lQuote - 1);

    if (tag == "Event") {
        _event = value;
    } else if (tag == "Site") {
        _site = value;
    } else if (tag == "Round") {
        _round = value;
    } else if (tag == "Variant") {
        _variant = value;
    } else if (tag == "Date") {
        _date = value;
    } else if (tag == "White") {
        player1->setName(value);
    } else if (tag == "Black") {
        player1->setName(value);
    } else {
        _otherTags[tag] = value;
    }
}

template<typename P1, typename P2>
int Server::play(int argc, char *argv[], P1 *p1, P2 *p2) {
    Scope scope{"Server::play"};

    QApplication app(argc, argv);
    MainWindow mw{};

    QThread *thread1 = new QThread;
    QThread *thread2 = new QThread;

    p1->moveToThread(thread1);
    p2->moveToThread(thread2);

    connectServerToPlayers(this, p1, p2);
    p2->setGui(&mw);
    connectPlayerToGui(p2, p2->gui());
    p2->showGui();

    thread1->start();
    thread2->start();

    initializeBoard(Glinski::fenInitial);

    Color firstMover = board.mover();
    if (firstMover == Color::Black) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "Calling Server::sendActionRequestToPlayer2\n");
        }
        sendActionRequestToPlayer2(
            firstMover, board.getLegalMoves(firstMover));
    } else {
        if (hexchess::events_verbose) {
            print(cout, scope(), "Calling Server::sendActionRequestToPlayer1\n");
        }
    }
    sendActionRequestToPlayer1(
        firstMover, board.getLegalMoves(firstMover));
    return app.exec();
}

}  // namespace hexchess::server

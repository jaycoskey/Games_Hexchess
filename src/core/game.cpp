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

#include <iostream>
#include <memory>
#include <regex>
#include <stdexcept>

#include <Qt>

#include "game.h"
#include "game_outcome.h"
#include "util_hexchess.h"

using std::cout;
using std::string;

using std::vector;


namespace hexchess::core {

using hexchess::split;

// TODO: Add two shared_ptr<Player> args and an optional FEN string to Game::Game()
Game::Game(bool doPopulate)
    : outcome{Termination::None},
      _event{"Game played on Hexchess"},
      _site{"Virtual"},
      _round{"1"},
      _variant{"Glinski"}
{
    hexchess::events_verbose = true;
    bool verbose = false;

    player1 = std::make_shared<PlayerRandom>();
    player2 = std::make_shared<PlayerRandom>();
    board = Board<Glinski>{doPopulate};

    // Set date and time strings
    time_t unixTime = time(&unixTime);
    struct tm* timeInfo = gmtime(&unixTime);
    char buffer[100];

    // Note: Could instead use std::put_time (since C++11)
    strftime(buffer, sizeof(buffer), "%Y.%m.%d", timeInfo);
    string date_{buffer, buffer + 10};
    if (verbose) { cout << "Current date = <<" << date_ << ">>\n"; }

    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeInfo);
    string time_{buffer, buffer + 8};
    if (verbose) { cout << "Current time (UTC) = <<" << time_ << ">>\n"; }
    // \todo Use current date and time in saving games in PGN format
}

std::shared_ptr<Player> Game::getPlayer(Color c) {
    return c == Color::Black ? player2 : player1;
}

void Game::setPlayer1(std::shared_ptr<Player>& p1) { player1 = p1; }
void Game::setPlayer2(std::shared_ptr<Player>& p2) { player2 = p2; }

const std::string Game::playerName(Color c) const {
    return c == Color::Black ? player2->name() : player1->name();
}

void Game::receiveActionFromPlayer(Color mover, PlayerAction action)
{
    bool verbose = false;
    if (verbose) {
        std::cout << "Server receives from player " << mover << " the action: "
                  << action.player_action_string(false) << "\n";
    }
    switch(action.playerActionEnum()) {
    case PlayerActionEnum::Move:
        if (verbose) {
        std::cout << "\tChecking if it's one of the "
                  << board.getLegalMoves(mover).size()
                  << " legal moves\n";
        }
        // \todo Speed up with hash (or multi-level map: PieceType->Moves)
        for (Move& legalMove : board.getLegalMoves(mover)) {
            if (legalMove == action.move()) {
                if (verbose) {
                    cout << "Game::receiveActionFromPlayer: Found that it's legal (among "
                         << board.getLegalMoves(mover).size() << ")\n";
                    cout << "Server executing move #"
                         << board.currentCounter() << "\n";
                }
                if (verbose) {
                    cout << "Server executing action: "
                         << action.player_action_string(false) << "\n";
                }
                board.moveExec(legalMove);

                if (board.getOptOutcome() != std::nullopt) {
                    GameOutcome outcome = board.getOptOutcome().value();
                    sendGameOutcomeToPlayer1(Color::White, outcome);
                    sendGameOutcomeToPlayer2(Color::Black, outcome);
                    // TODO: Handle end of game
                }
                if (board.isCheck()) {
                    sendCheckToPlayers(mover, board.getKingIndex(mover));
                }
                Color nextMover = opponent(mover);

                // Send action to mover's opponent
                if (hexchess::events_verbose) {
                    std::cout << "Server sending action to player "
                              << nextMover << "\n";
                }
                if (nextMover == Color::Black) {
                    sendActionToPlayer2(nextMover, action);
                } else {
                    sendActionToPlayer1(nextMover, action);
                }

                // Send action request to next player
                if (hexchess::events_verbose) {
                    std::cout << "Server sending action request to player "
                              << nextMover << "\n";
                }
                if (nextMover == Color::Black) {
                    sendActionRequestToPlayer2(nextMover);
                } else {
                    sendActionRequestToPlayer1(nextMover);
                }
            }
        }
        break;
    case PlayerActionEnum::Draw_Offer:
        throw new NotImplementedException("Game:receiveActionFromPlayer: action=Draw_Offer");
        break;
    case PlayerActionEnum::Draw_Accept:
        throw new NotImplementedException("Game:receiveActionFromPlayer: action=Draw_Accept");
        break;
    case PlayerActionEnum::Draw_Decline:
        throw new NotImplementedException("Game:receiveActionFromPlayer: action=Draw_Decline");
        break;
    case PlayerActionEnum::Resign:
        throw new NotImplementedException("Game:receiveActionFromPlayer: action=Draw_Resign");
        // TODO: Implement receivePlayerActionFromPlayer: Resign
        break;
    default:
        throw std::logic_error("receivePlayerActionFromPlayer: Unrecognized player action");
    }
}

/// \todo Add Game::game_outcome_json_string()
const string Game::game_summary_string() const {
    std::ostringstream oss;

    oss << "Players=("
        <<   "White:\"" <<  player1->name() << "\", "
        <<   "Black:\"" <<  player2->name() << "\", "
        <<   "), "
        << "Outcome="
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
void Game::load_pgn(const string& pgn) {
    assert(board.currentCounter() == 0);
    assert(pgn.size() > 0);

    std::stringstream ss{pgn};
    std::string line;

    cout << board.board_string() << "\n";

    // \todo Make handling of line endings cross-platform
    while (std::getline(ss, line, '\n')) {
        if (line[0] == '#') {
            continue;
        }
        if (line[0] == '[') {
            _pgn_tag_parse(line);
        } else if (line[0] == '1') {
            for (const std::string& moveStr : split(' ', line)) {
                if (isdigit(moveStr[0])) {
                    continue;
                }
                std::smatch moveMatch;
                string moveRegexStr{
                    "([a-zA-Z]\{1,2\}[0-9]\{1,2\})"  // fromStr
                    "([x-]?)"                   // infix
                    "([a-zA-Z][0-9]\{1,2\})"         // toStr
                    "(.*)"                      // extras
                    };
                std::regex moveRegex{moveRegexStr, std::regex::extended};
                if (!std::regex_search(moveStr, moveMatch, moveRegex)) {
                    cout << "Game::load_pgn: Failed to match "
                         << moveStr << " against " << moveRegexStr << "\n";
                    throw std::invalid_argument(string{"Game::load_pgn: "} + moveStr);
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
                    Index fromInd = V::cellNameToIndex(fromStr);
                    pt = board.getPieceTypeAt(fromInd);
                }
                Index from = V::cellNameToIndex(fromStr);
                Index to = V::cellNameToIndex(toStr);
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
                board.moveExec(move);
            }
        }
    }
}

const string Game::game_pgn_string() const {
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
void Game::_pgn_tag_parse(const string& line) {
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

}  // namespace hexchess::core

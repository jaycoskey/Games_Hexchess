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

#include <iostream>
#include <memory>

#include "game.h"
#include "game_outcome.h"
#include "util_hexchess.h"

using std::cout;
using std::string;


namespace hexchess::core {


// TODO: Add two shared_ptr<Player> args and an optional FEN string to Game::Game()
Game::Game(bool doPopulate)
    : outcome{Termination::None},
      _event{"Game played on Hexchess"},
      _site{"Virtual"},
      _round{"1"},
      _variant{"Glinski"}
{
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
    cout << "Current date = <<" << date_ << ">>\n";

    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeInfo);
    string time_{buffer, buffer + 8};
    cout << "Current time (UTC) = <<" << time_ << ">>\n";
}

std::shared_ptr<Player> Game::getPlayer(Color c) {
    return c == Color::Black ? player2 : player1;
}

void Game::setPlayer1(std::shared_ptr<Player>& p1) { player1 = p1; }
void Game::setPlayer2(std::shared_ptr<Player>& p2) { player2 = p2; }

const std::string Game::playerName(Color c) const {
    return c == Color::Black ? player2->name() : player1->name();
}

void Game::receiveActionFromPlayer(Color mover, PlayerAction& action) {
    switch(action.playerActionEnum()) {
    case PlayerActionEnum::Move:
        for (Move& legalMove : board.getLegalMoves(mover)) {
            if (legalMove == action.move()) {
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
                if (nextMover == Color::Black) {
                    sendActionToPlayer2(nextMover, action);
                } else {
                    sendActionToPlayer1(nextMover, action);
                }
            }
        }
        break;
    case PlayerActionEnum::Offer_Draw:
        // TODO: Implement receivePlayerActionFromPlayer: Offer_Draw
        break;
    case PlayerActionEnum::Offer_Draw_Accept:
        // TODO: Implement receivePlayerActionFromPlayer: Offer_Draw_Accept
        break;
    case PlayerActionEnum::Offer_Draw_Decline:
        // TODO: Implement receivePlayerActionFromPlayer: Offer_Draw_Decline
        break;
    case PlayerActionEnum::Resign:
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
        oss << "Win for " << color_string(outcome.winner())
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

const string Game::pgn_string() const {
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
    oss << board.board_pgn_string();

    return oss.str();
}

}  // namespace hexchess::core

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

#pragma once

#include <ctime>
#include <time.h>

#include <iostream>
#include <memory>

#include <QObject>

#include "board.h"
#include "game_outcome.h"
#include "player.h"
#include "player_simple_random.h"
#include "util_hexchess.h"


namespace hexchess::core {

using player::Player;
using player::PlayerRandom;

class Game : public QObject {
    Q_OBJECT
    typedef Glinski V;

public:
    Game(bool doPopulate=true);

    const GameOutcome& play();

    std::shared_ptr<Player> getPlayer(Color c);
    void setPlayer1(std::shared_ptr<Player>& p1);
    void setPlayer2(std::shared_ptr<Player>& p2);
    const std::string playerName(Color c) const;

    const std::string game_summary_string() const;
    const std::string pgn_string() const;

    Board<V> board;
    GameOutcome outcome;
    std::shared_ptr<Player> player1;
    std::shared_ptr<Player> player2;

public slots:
    void receiveActionFromPlayer(Color mover, PlayerAction& action);

signals:
    // Boardcast to Players
    void sendBoardInitializationToPlayers(const Fen<V>& fen);
    void sendCheckToPlayers(Color checked, Index kingInd);
    void sendGameOutcomeToPlayer1(Color reader, const GameOutcome& outcome);
    void sendGameOutcomeToPlayer2(Color reader, const GameOutcome& outcome);

    // Sent individually
    void sendActionRequestToPlayer1(Color mover);
    void sendActionRequestToPlayer2(Color mover);

    void sendActionToPlayer1(Color mover, PlayerAction& action);
    void sendActionToPlayer2(Color mover, PlayerAction& action);

private:
    void _announceGameEnd(const GameOutcome &outcome) const;
    void _printGameStats() const;

    // ---------- Private write methods
    // void _initPlayers();
    void _reset();

    std::string _event;
    std::string _site;
    std::string _round;
    std::string _variant;

    std::string _date;
    std::string _time;
};

}  // namespace hexchess::core

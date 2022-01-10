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
#include <map>
#include <memory>

#include <QObject>
#include <QThread>

#include "board.h"
#include "game_outcome.h"
#include "player.h"
#include "player_random.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::server {

using core::Board;
using core::Color;
using core::Fen;
using core::GameOutcome;
using core::Index;
using core::Moves;

using player::Player;
using player::PlayerRandom;

class Server : public QObject {
    Q_OBJECT
    // typedef Glinski V;

public:
    Server();
    // ~Server();
    Server(const Server& other);

    const GameOutcome& play();

    Player* getPlayer(Color c);
    void setPlayer1(Player *p1);
    void setPlayer2(Player *p2);
    void initializeBoard(const Fen<Glinski>& fen);
    void initializeBoard(const std::string& fenStr);

    const std::string playerName(Color c) const;
    const std::string game_summary_string() const;
    void load_pgn(const std::string& pgn);
    const std::string game_pgn_string() const;

    bool getIsGameOver() const { return board.getIsGameOver(); }

    template<typename P1, typename P2>
    int play(int argc, char *argv[], P1 *p1, P2 *p2);

    Board<Glinski> board;
    GameOutcome outcome;
    Player *player1;
    Player *player2;

public slots:
    void receiveActionFromPlayer(Color mover, PlayerAction action);

signals:
    // Boardcast to Players
    void sendBoardInitializationToPlayers(const Fen<Glinski>& fen);
    void sendCheckToPlayers(Color checked, Index kingInd);
    void sendGameOutcomeToPlayer1(Color reader, const GameOutcome& outcome);
    void sendGameOutcomeToPlayer2(Color reader, const GameOutcome& outcome);

    // Sent individually
    void sendActionRequestToPlayer1(Color mover, const Moves legalMoves);
    void sendActionRequestToPlayer2(Color mover, const Moves legalMoves);

    void sendActionToPlayer1(Color mover, const PlayerAction action);
    void sendActionToPlayer2(Color mover, const PlayerAction action);

protected:
    void _announceGameEnd(const GameOutcome &outcome) const;
    void _pgn_tag_parse(const std::string& line);
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

    std::map<std::string, std::string> _otherTags;
//    QThread *_thread;
};

template<typename SERVER, typename P1, typename P2>
int play(int argc, char *argv[], SERVER *server, P1 *p1, P2 *p2) {
    Scope scope{"main.cpp::play"};

    QApplication app(argc, argv);
    MainWindow mw1{};
    MainWindow mw2{};

    QThread *thread1 = new QThread;
    QThread *thread2 = new QThread;

    p1->moveToThread(thread1);
    p2->moveToThread(thread2);

    connectServerToPlayers(server, p1, p2);

    p1->setGui(&mw1);
    connectPlayerToGui(p1, p1->gui());
    p1->showGui();

    p2->setGui(&mw2);
    connectPlayerToGui(p2, p2->gui());
    p2->showGui();

    thread1->start();
    thread2->start();

    server->initializeBoard(Glinski::fenInitial);

    Color firstMover = server->board.mover();
    if (firstMover == Color::Black) {
        if (hexchess::events_verbose) {
            print(cout, scope(), "Calling Server::sendActionRequestToPlayer2\n");
    }
        server->sendActionRequestToPlayer2(
            firstMover, server->board.getLegalMoves(firstMover));
    } else {
        if (hexchess::events_verbose) {
            print(cout, scope(), "Calling Server::sendActionRequestToPlayer1\n");
        }
    }
    server->sendActionRequestToPlayer1(
        firstMover, server->board.getLegalMoves(firstMover));
    return app.exec();
}

}  // namespace hexchess::server

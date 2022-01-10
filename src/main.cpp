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

#include <cstdlib>
#include <cstring>

#include <fstream>
#include <iostream>
#include <streambuf>

#include <QApplication>
#include <QMetaType>
#include <QObject>
#include <QtConcurrent>
#include <QThread>

#include "board.h"
#include "move.h"
#include "player.h"
#include "player_action.h"
#include "player_alpha_beta.h"
#include "player_human_text.h"
#include "player_random.h"
#include "server.h"
#include "server_thread.h"
#include "util_hexchess.h"
#include "version.h"

#include "ui/boardwidget.h"
#include "ui/mainwindow.h"

using std::cout;

using hexchess::print;
using hexchess::version_string;

using hexchess::core::Color;
using hexchess::core::Moves;
using hexchess::core::PlayerAction;
using hexchess::core::Scope;
using hexchess::core::Short;
using hexchess::core::staticMetaObject;

using hexchess::player::connectServerToPlayers;
using hexchess::player::Player;
using hexchess::player::PlayerAlphaBeta;
using hexchess::player::PlayerHumanText;
using hexchess::player::PlayerRandom;

using hexchess::server::Server;
using hexchess::server::ServerThread;

using Fen_Glinski = Fen<Glinski>;

const char* GLINSKI_DEMO_GAME1{"../resources/games/pgn/Bodor_Hexodus_1999.pgn"};
const char* GLINSKI_DEMO_GAME2{"../resources/games/pgn/Mackowiak_Hexodus_1999.pgn"};
const char* GLINSKI_DEMO_GAME3{"../resources/games/pgn/Schenkerik_Hexodus_1999.pgn"};

/// \todo Modify to support multiple variants
int main(int argc, char *argv[]) {
    const Scope scope{"main.cpp:main"};
    hexchess::events_verbose = true;
    hexchess::general_verbose = true;

    qRegisterMetaType<Color>("Color");
    qRegisterMetaType<Fen_Glinski>("Fen<Glinski>");
    qRegisterMetaType<Moves>("Moves");
    qRegisterMetaType<PlayerAction>("PlayerAction");

    // qRegisterMetaType<PlayerThread<PlayerRandom>>("PlayerThread_PlayerRandom");
    QApplication app(argc, argv);

    Short sleepTimeMs = 10;

    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        cout << "Version: " << version_string() << "\n";
        exit(0);
    }
    else if (argc == 2 && strcmp(argv[1], "--test_ab") == 0) {
        using PAB = PlayerAlphaBeta;

        PAB p1{"AB_White"};
        PAB p2{"AB_Black"};

        ServerThread server{};
        server.setPlayer1(&p1);
        server.setPlayer2(&p2);

        connectServerToPlayers(&server, &p1, &p2);
        MainWindow mw{};
        p1.setGui(&mw);
        connectPlayerToGui<PAB>(&p1, p1.gui());
        p1.showGui();

        server.initializeBoard(Glinski::fenInitial);

        Color firstMover = server.board.mover();
        if (firstMover == Color::Black) {
            if (hexchess::events_verbose) {
                print(cout, scope(), "Sending action request to player #2\n");
            }
            server.sendActionRequestToPlayer2(
                firstMover, server.board.getLegalMoves(firstMover));
            if (hexchess::events_verbose) {
                print(cout, scope(), "Sent action request to player #2\n");
            }
        } else {
            if (hexchess::events_verbose) {
                print(cout, scope(), "Sending action request to player #1\n");
            }
            server.sendActionRequestToPlayer1(
                firstMover, server.board.getLegalMoves(firstMover));
            if (hexchess::events_verbose) {
                print(cout, scope(), "Sent action request to player #1\n");
            }
        }
        return app.exec();
    }
    else if (argc == 2 && strcmp(argv[1], "--test_load") == 0) {
        using PHT = PlayerHumanText;

        PHT p1{};
        PHT p2{};
        p1.setName("Human_White");
        p2.setName("Human_Black");
        ServerThread server{};
        server.setPlayer1(&p1);
        server.setPlayer2(&p2);
        connectServerToPlayers(&server, &p1, &p2);

        MainWindow mw{};
        p1.setGui(&mw);
        connectPlayerToGui<PHT>(&p1, p1.gui());
        p1.showGui();

        std::ifstream pgnStream{GLINSKI_DEMO_GAME1};
        std::string pgnStr{
            std::istreambuf_iterator<char>(pgnStream),
            std::istreambuf_iterator<char>()};
        cout << "pgnStr=" << pgnStr << "\n";
        server.load_pgn(pgnStr);  // Initializes gameP->board
        (void) server.board.getLegalMoves(server.board.mover());
        (void) server.board.getOptOutcome();

        cout << "Entering game loop\n";
        while (!server.getIsGameOver()) {
            Color mover = server.board.mover();
            if (mover == Color::Black) {
                server.sendActionRequestToPlayer2(mover, server.board.getLegalMoves(mover));
            } else {
                server.sendActionRequestToPlayer1(mover, server.board.getLegalMoves(mover));
            }
        }
        return app.exec();
    }
    else if (argc == 2 && strcmp(argv[1], "--test_random") == 0) {
        ServerThread server{};
        PlayerRandom p1{"White"};
        PlayerRandom p2{"Black"};
        return play(argc, argv, &server, &p1, &p2);
    }
    else {
        // GUI --- Just display board with starting layout.
        using PR = PlayerRandom;

        PR p1{};
        PR p2{};
        p1.setName("Random_White");
        p2.setName("Random_Black");
        ServerThread server{};
        server.setPlayer1(&p1);
        server.setPlayer2(&p2);
        connectServerToPlayers(&server, &p1, &p2);

        MainWindow mw{};
        p1.setGui(&mw);
        connectPlayerToGui<PR>(&p1, server.player1->gui());
        p1.showGui();

        server.initializeBoard(Glinski::fenInitial);
        (void) server.board.getLegalMoves(server.board.mover());
        (void) server.board.getOptOutcome();

        print(cout, scope(), "Board has ", server.board.pieceCount(), " pieces\n");
        app.exec();
    }
    return 0;  // NOT_REACHED
}

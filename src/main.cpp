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

#include <cstring>

#include <iostream>


#include "board.h"
#include "game.h"
#include "player.h"
#include "player_human_text.h"
#include "player_simple_random.h"
#include "version.h"

#include "ui/boardwidget.h"
#include "ui/mainwindow.h"

using std::cout;

using hexchess::version_string;

using hexchess::core::Color;
using hexchess::core::Game;

using hexchess::player::connectSignalsToSlots;
using hexchess::player::Player;
using hexchess::player::PlayerHumanText;
using hexchess::player::PlayerRandom;

/// \todo Modify to support multiple variants
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Game game{false};
    MainWindow mw{};

    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        cout << "Version: " << version_string() << "\n";
    } else if (argc == 2 && strcmp(argv[1], "--text") == 0) {
        std::shared_ptr<Player> player1 = std::make_shared<PlayerHumanText>();
        std::shared_ptr<Player> player2 = std::make_shared<PlayerHumanText>();
        game.setPlayer1(player1);
        game.setPlayer2(player2);

        PlayerHumanText *p1 = dynamic_cast<PlayerHumanText*>(player1.get());
        PlayerHumanText *p2 = dynamic_cast<PlayerHumanText*>(player2.get());

        hexchess::player::connectSignalsToSlots(game, p1, p2);
        game.sendBoardInitializationToPlayers(Glinski::fenInitial);
        game.sendActionRequestToPlayer1(Color::White);
    } else {
        std::shared_ptr<Player> player1 = std::make_shared<PlayerRandom>();
        std::shared_ptr<Player> player2 = std::make_shared<PlayerRandom>();
        game.setPlayer1(player1);
        game.setPlayer2(player2);

        PlayerRandom *p1 = dynamic_cast<PlayerRandom*>(player1.get());
        PlayerRandom *p2 = dynamic_cast<PlayerRandom*>(player2.get());

        hexchess::player::connectSignalsToSlots(game, p1, p2);
        mw.show();
    }
    return a.exec();
}

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

#include <iostream>
#include <string>

#include <QObject>
#include <Qt>

#include "board.h"
#include "fen.h"
#include "player_action.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"

#include "ui/mainwindow.h"

using std::cout;


namespace hexchess::player {

using core::Board;
using core::CheckEnum;
using core::Color;
using core::Fen;
using core::GameOutcome;
using core::Glinski;
using core::Index;
using core::Move;
using core::Moves;
using core::PieceType;
using core::PlayerAction;
using core::Scope;
using core::Value;

class Player : public QObject {
    Q_OBJECT

public:
    typedef Glinski V;

    Player() {};
    virtual ~Player() {};

    virtual bool isHuman() const = 0;
    virtual const std::string name() const = 0;
    virtual void setName(std::string name) = 0;

    virtual MainWindow* gui() const = 0;
    virtual void setGui(MainWindow *mwp) = 0;
    virtual void showGui() const = 0;

public slots:
    // ========================================
    // SERVER <--> Player

    // Broadcast
    virtual void receiveBoardInitializationFromServer(const Fen<Glinski>& fen) = 0;
    virtual void receiveCheckFromServer(Color checked, Index kingInd) = 0;

    // Sent individually
    virtual void receiveActionRequestFromServer(Color mover, const Moves legalMoves) = 0;
    virtual void receiveActionFromServer(Color mover, const PlayerAction action) = 0;
    virtual void receiveGameOutcomeFromServer(Color receiver, const GameOutcome& GameOutcome) = 0;

    // TODO: Receive DrawOffer, DrawAcceptance, or DrawDecline
    // TODO: Receive "superuser" board edits: addPiece, movePiece, removePiece

    // ========================================
    // Player <--> GUI

    virtual void receiveActionFromGui(Color mover, const PlayerAction& action) = 0;

signals:
    // ========================================
    // SERVER <--> Player

    void sendActionToServer(Color mover, PlayerAction action);
    // TODO: Send DrawOffer, DrawAcceptance, or DrawDecline
    // TODO: Send "superuser" board edits: addPiece, movePiece, removePiece

    // ========================================
    // Player <--> GUI
    void sendBoardInitializationToGui(const Fen<Glinski>& fen);

    void sendActionRequestToGui(Color mover, const Moves& legalMoves);
    void sendActionToGui(Color mover, const PlayerAction& action);
    void sendCheckToGui(Color checked, Index kingInd);
    void sendGameOutcomeToGui(Color receiver, const GameOutcome& GameOutcome);
};

template <typename SERVER, typename P1, typename P2>
void connectServerToPlayers(SERVER *server, P1 *p1, P2 *p2) {
    Scope scope{"connectServerToPlayers", true};

    // Sent from server to players collectively
    assert(QObject::connect(server, &SERVER::sendBoardInitializationToPlayers,
                            p1,     &P1::receiveBoardInitializationFromServer));

    assert(QObject::connect(server, &SERVER::sendBoardInitializationToPlayers,
                            p2,     &P2::receiveBoardInitializationFromServer));

    assert(QObject::connect(server, &SERVER::sendCheckToPlayers,
                            p1,     &P1::receiveCheckFromServer));

    assert(QObject::connect(server, &SERVER::sendCheckToPlayers,
                            p2,     &P2::receiveCheckFromServer));

    // Sent from server to players individually
    assert(QObject::connect(server, &SERVER::sendGameOutcomeToPlayer1,
                            p1,     &P1::receiveGameOutcomeFromServer));

    assert(QObject::connect(server, &SERVER::sendGameOutcomeToPlayer2,
                            p2,     &P2::receiveGameOutcomeFromServer));

    assert(QObject::connect(server, &SERVER::sendActionRequestToPlayer1,
                            p1,     &P1::receiveActionRequestFromServer));

    assert(QObject::connect(server, &SERVER::sendActionRequestToPlayer2,
                            p2,     &P2::receiveActionRequestFromServer));

    assert(QObject::connect(server, &SERVER::sendActionToPlayer1,
                            p1,     &P1::receiveActionFromServer));
                            // Qt::QueuedConnection));

    assert(QObject::connect(server, &SERVER::sendActionToPlayer2,
                            p2,     &P2::receiveActionFromServer));
                            // Qt::QueuedConnection));

    // Sent from player to server
    assert(QObject::connect(p1,     &P1::sendActionToServer,
                            server, &SERVER::receiveActionFromPlayer));
                            // Qt::QueuedConnection));

    assert(QObject::connect(p2,     &P2::sendActionToServer,
                            server, &SERVER::receiveActionFromPlayer));
                            // Qt::QueuedConnection));

    print(std::cout, scope("Exiting"), "\n");
}

template <typename P, typename GUI>
void connectPlayerToGui(P *player, GUI *gui) {
    Scope scope{"Player::connectPlayerToGui", true};

    // From Player to GUI
    QObject::connect(player, &P::sendBoardInitializationToGui,
                     gui,    &GUI::receiveBoardInitializationFromPlayer);

    QObject::connect(player, &P::sendActionRequestToGui,
                     gui,    &GUI::receiveActionRequestFromPlayer);
    QObject::connect(player, &P::sendActionToGui,
                     gui,    &GUI::receiveActionFromPlayer);
    QObject::connect(player, &P::sendCheckToGui,
                     gui,    &GUI::receiveCheckFromPlayer);
    QObject::connect(player, &P::sendGameOutcomeToGui,
                     gui,    &GUI::receiveGameOutcomeFromPlayer);

    // Sent from GUI to Player
    QObject::connect(gui,    &GUI::sendActionToPlayer,
                     player, &P::receiveActionFromGui);

    print(std::cout, scope("Exiting"), "\n");
}

}  // namespace hexchess::player

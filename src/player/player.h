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

#include <string>

#include <QObject>

#include "board.h"
#include "fen.h"
#include "player_action.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"


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
using core::Value;

class Player : public QObject {
    Q_OBJECT

public:
    typedef Glinski V;

    Player() {};
    virtual ~Player() {};

    virtual const std::string name() const = 0;
    virtual void setName(std::string name) = 0;

public slots:
    // Broadcast
    virtual void initializeBoard(const Fen<V>& fen) = 0;
    virtual void showCheckToPlayer(Color checked, Index kingInd) = 0;

    // Sent individually
    virtual void showActionRequestToPlayer(Color mover) = 0;
    virtual void showActionToPlayer(Color mover, PlayerAction& action) = 0;
    virtual void showGameOutcomeToPlayer(Color reader, const GameOutcome& gameOutcome) = 0;

    // TODO: Receive DrawOffer, DrawAcceptance, or DrawDecline
    // TODO: Receive "superuser" board edits: addPiece, movePiece, removePiece

signals:
    void sendActionToServer(Color mover, PlayerAction& action);
    // TODO: Send DrawOffer, DrawAcceptance, or DrawDecline
    // TODO: Send "superuser" board edits: addPiece, movePiece, removePiece
};

template <typename GAME, typename P1, typename P2>
void connectSignalsToSlots(const GAME& game, P1 *p1, P2 *p2) {
    // Broadcast from server to players
    QObject::connect(&game, &GAME::sendBoardInitializationToPlayers,
                     p1,    &P1::initializeBoard);
    QObject::connect(&game, &GAME::sendBoardInitializationToPlayers,
                     p2,    &P2::initializeBoard);

    QObject::connect(&game, &GAME::sendCheckToPlayers,
                     p1,    &P1::showCheckToPlayer);
    QObject::connect(&game, &GAME::sendCheckToPlayers,
                     p2,    &P2::showCheckToPlayer);

    QObject::connect(&game, &GAME::sendGameOutcomeToPlayer1,
                     p1,    &P1::showGameOutcomeToPlayer);
    QObject::connect(&game, &GAME::sendGameOutcomeToPlayer2,
                     p2,    &P2::showGameOutcomeToPlayer);

    // Sent from server to players individually
    QObject::connect(&game, &GAME::sendActionRequestToPlayer1,
                     p1,    &P1::showActionRequestToPlayer);
    QObject::connect(&game, &GAME::sendActionRequestToPlayer2,
                     p2,    &P2::showActionRequestToPlayer);

    QObject::connect(&game, &GAME::sendActionToPlayer1,
                     p1,    &P1::showActionToPlayer);
    QObject::connect(&game, &GAME::sendActionToPlayer2,
                     p2,    &P2::showActionToPlayer);

    // Sent from player to server
    QObject::connect(p1,    &P1::sendActionToServer,
                     &game, &GAME::receiveActionFromPlayer);
    QObject::connect(p2,    &P2::sendActionToServer,
                     &game, &GAME::receiveActionFromPlayer);
}

}  // namespace hexchess::player

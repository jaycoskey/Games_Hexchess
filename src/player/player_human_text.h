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
#include "player.h"
#include "variant.h"


namespace hexchess::player {

using core::PlayerAction;

/// \brief Model Player with getNextAction function
///
/// \todo Add ability to take actions other than moving. For example, resignation or offering a Draw.
/// \todo Add support for slots & signals.
class PlayerHumanText : public Player {
    Q_OBJECT

public:
    typedef Glinski V;

    PlayerHumanText() : _board{false} {}
    virtual ~PlayerHumanText() override {};

    virtual const std::string name() const override { return "PlayerHumanText"; }

public slots:
    // Broadcast
    virtual void initializeBoard(const Fen<V>& fen) override;
    virtual void showCheckToPlayer(Color checked, Index kingInd) override;

    // Sent individually
    virtual void showActionRequestToPlayer(Color mover) override;
    virtual void showActionToPlayer(Color mover, PlayerAction& action) override;
    virtual void showGameOutcomeToPlayer(Color reader, const GameOutcome& gameOutcome) override;

    // TODO: Receive DrawOffer, DrawAcceptance, or DrawDecline
    // TODO: Receive "superuser" board edits: addPiece, movePiece, removePiece

signals:
    void sendActionToServer(Color mover, PlayerAction& action);
    // TODO: Send DrawOffer, DrawAcceptance, or DrawDecline
    // TODO: Send "superuser" board edits: addPiece, movePiece, removePiece

private:
    static const std::string _helpMessage;

    Board<Glinski> _board;
};

}  // namespace hexchess::player

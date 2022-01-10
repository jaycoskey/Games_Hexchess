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

#include <QMetaObject>
#include <QObject>

#include "board.h"
#include "player.h"
#include "variant.h"

#include "ui/mainwindow.h"


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

    PlayerHumanText() : _board{"PlayerHumanText", false} {}
    virtual ~PlayerHumanText() override {};

    virtual bool isHuman() const override { return true; }
    virtual const std::string name() const override { return _name; }
    virtual void setName(std::string name) override { _name = name; }

    virtual MainWindow* gui() const override { return _gui; }
    virtual void setGui(MainWindow *mwp) override { cout << "PlayerHumanText::setGui: Entering\n"; _gui = mwp; }
    virtual void showGui() const override { _gui->show(); }

public slots:
    // ========================================
    // Game <--> Player

    // Broadcast
    virtual void receiveBoardInitializationFromServer(const Fen<V>& fen) override;
    virtual void receiveCheckFromServer(Color checked, Index kingInd) override;

    // Sent individually
    virtual void receiveActionRequestFromServer(Color mover, const Moves legalMoves) override;
    virtual void receiveActionFromServer(Color mover, const PlayerAction action) override;
    virtual void receiveGameOutcomeFromServer(Color receiver, const GameOutcome& gameOutcome) override;

    // TODO: Receive DrawOffer, DrawAcceptance, or DrawDecline
    // TODO: Receive "superuser" board edits: addPiece, movePiece, removePiece

    // ========================================
    // Player <--> GUI

    virtual void receiveActionFromGui(Color mover, const PlayerAction& action) override;

signals:
    // ========================================
    // Game <--> Player

    void sendActionToServer(Color mover, const PlayerAction action);
    // TODO: Send DrawOffer, DrawAcceptance, or DrawDecline
    // TODO: Send "superuser" board edits: addPiece, movePiece, removePiece

    // ========================================
    // Player <--> GUI
    void sendBoardInitializationToGui(const Fen<V>& fen);

    void sendActionRequestToGui(Color mover, const Moves legalMoves);
    void sendActionToGui(Color mover, const PlayerAction action);
    void sendCheckToGui(Color checked, Index kingInd);
    void sendGameOutcomeToGui(Color receiver, const GameOutcome& gameOutcome);

protected:
    static const std::string _helpMessage;

    Board<Glinski> _board;
    MainWindow* _gui;
    std::string _name;
};

}  // namespace hexchess::player

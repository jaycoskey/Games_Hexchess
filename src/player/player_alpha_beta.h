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

#include <QObject>

#include "board.h"
#include "fen.h"
#include "game_outcome.h"
#include "player.h"
#include "player_action.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"

#include "ui/mainwindow.h"


namespace hexchess::player {

using core::Board;
using core::Color;
using core::Fen;
using core::GameOutcome;
using core::Index;
using core::Move;
using core::Moves;
using core::PlayerAction;
using core::Glinski;
using core::Short;
using core::Value;

class PlayerAlphaBeta : public Player {
    Q_OBJECT

public:
    typedef Glinski V;

    PlayerAlphaBeta(Short minDepth=3)
        : _minDepth{minDepth},
          _name{"PlayerAlphaBeta"}
    {}
    PlayerAlphaBeta(const std::string& name, Short minDepth=3)
        : _minDepth{minDepth},
          _name{name}
    {}
    virtual ~PlayerAlphaBeta() override {};

    virtual bool isHuman() const override { return false; }
    virtual const std::string name() const override { return _name; }
    virtual void setName(std::string name) override { _name = name; }

    virtual MainWindow* gui() const override { return _gui; }
    virtual void setGui(MainWindow *mwp) override { _gui = mwp; }
    virtual void showGui() const override { _gui->show(); }

public slots:
    // ========================================
    // Game <--> Player

    // Broadcast
    virtual void receiveBoardInitializationFromServer(const Fen<Glinski>& fen) override;
    virtual void receiveCheckFromServer(Color checked, Index kingInd) override;

    // Sent individually
    virtual void receiveActionRequestFromServer(Color mover, const Moves legalMoves) override;
    virtual void receiveActionFromServer(Color mover, const PlayerAction action) override;
    virtual void receiveGameOutcomeFromServer(Color receiver, const GameOutcome& gameOutcome) override;

    // ========================================
    // Player <--> GUI

    virtual void receiveActionFromGui(Color mover, const PlayerAction& action) override;

signals:
    // ========================================
    // Game <--> Player

    void sendActionToServer(Color mover, PlayerAction action);

    // ========================================
    // Player <--> GUI
    void sendBoardInitializationToGui(const Fen<Glinski>& fen);

    void sendActionRequestToGui(Color mover, const Moves legalMoves);
    void sendActionToGui(Color mover, const PlayerAction& action);
    void sendCheckToGui(Color checked, Index kingInd);
    void sendGameOutcomeToGui(Color receiver, const GameOutcome& gameOutcome);

protected:
    Board<Glinski> _board{"PlayerAlphaBeta", false};
    MainWindow* _gui;
    Short _minDepth;
    std::string _name;
};

}  // namespace hexchess::player

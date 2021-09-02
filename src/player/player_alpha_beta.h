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

    PlayerAlphaBeta(Short min_depth=3) {}
    virtual ~PlayerAlphaBeta() override {};

    virtual const std::string name() const override {
        return "PlayerAlphaBeta";
    }

public slots:
    // Broadcast
    virtual void initializeBoard(const Fen<V>& fen) = 0;
    virtual void showCheckToPlayer(Color checked, Index kingInd) = 0;

    // Sent individually
    virtual void showActionRequestToPlayer(Color mover) = 0;
    virtual void showActionToPlayer(Color mover, PlayerAction& action) = 0;
    virtual void showGameOutcomeToPlayer(Color reader, const GameOutcome& gameOutcome) = 0;

signals:
    void sendActionToServer(Color mover, PlayerAction& action);

private:
    Board<Glinski> _board{false};
};

}  // namespace hexchess::player

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
#include "player.h"
#include "player_action.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::player {

using core::Board;
using core::Fen;
using core::Move;
using core::Moves;
using core::Value;
using core::PlayerAction;
using core::Glinski;

Value advancingMoveValuator(const Board<Glinski>& b, const Move& m);

class PlayerAdvancing : public Player {
    Q_OBJECT

public:
    typedef Glinski V;

    PlayerAdvancing() {}
    virtual ~PlayerAdvancing() override {}

    virtual const std::string name() const override { return "PlayerAdvancing"; }

public slots:
    // ========================================
    // Broadcast
    virtual void initializeBoard(const Fen<V>& fen) override;
    virtual void showCheckToPlayer(Color checked, Index kingInd) override;

    // ========================================
    // Sent individually
    virtual void showActionRequestToPlayer(Color mover) override;
    virtual void showActionToPlayer(Color mover, PlayerAction& action) override;
    virtual void showGameOutcomeToPlayer(Color reader, const GameOutcome& gameOutcome) override;

signals:
    virtual void sendActionToServer(Color mover, PlayerAction& action) override;

private:
    Board<V> _board{false};
};

}  // namespace hexchess::player
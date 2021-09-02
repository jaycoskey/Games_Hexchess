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

#include "board.h"
#include "fen.h"
#include "player.h"
#include "player_action.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::player {

using core::Board;
using core::Color;
using core::Fen;
using core::Move;
using core::PlayerAction;
using core::PlayerActionEnum;
using core::Glinski;

class PlayerRandom : public Player {
    Q_OBJECT

public:
    typedef Glinski V;

    PlayerRandom() {}
    virtual ~PlayerRandom() override {}

    virtual const std::string name() const override { return "PlayerRandom"; }

public slots:
    // ========================================
    // Broadcast
    virtual void initializeBoard(const Fen<V>& fen) override {
        _board.initialize(fen);
    }
    virtual void showCheckToPlayer(Color checked, Index kingInd) override {
        // No action needed; Computer player will detect check
    }

    // ========================================
    // Sent individually
    virtual void showActionRequestToPlayer(Color mover) override {
        const Move& move = chooseRandom(_board.getLegalMoves(_board.mover()));
        PlayerAction action{move};
        sendActionToServer(mover, action);
    }
    virtual void showActionToPlayer(Color mover, PlayerAction& action) override {
        // TODO: Support Draw offers, acceptances, and declines
        if (action.playerActionEnum() == PlayerActionEnum::Move) {
            _board.moveExec(action.move());
        }
    }
    virtual void showGameOutcomeToPlayer(Color reader, const GameOutcome& gameOutcome) override {
        // No action needed; Computer player will detect check
    }

signals:
    void sendActionToServer(Color mover, PlayerAction& action);

private:
    Board<V> _board{false};
};

}  // namespace hexchess::player

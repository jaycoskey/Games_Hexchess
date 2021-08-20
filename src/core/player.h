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

#include <map>

#include "board.h"
#include "fen.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"

using std::map;


namespace hexchess::core {

/// \brief Model Player with getNextMove function
///
/// \todo Add ability to take actions other than moving. For example, resignation or offering a Draw.
/// \todo Add support for slots & signals.
class Player {
public:
    typedef Glinski V;

    Player() {};
    virtual ~Player() {};

    virtual const std::string name() = 0;
    virtual const Move& getNextMove(const Fen<V>& fen) = 0;
    // getNextMove(const Board<V>& b) = 0;
    // getNextMove(const Board<V>& b, const Moves& legalMoves) = 0;
};

class RandomPlayer : public Player {
public:
    RandomPlayer() {}
    ~RandomPlayer() override {};

    const std::string name() override { return "RandomPlayer"; }

    const Move& getNextMove(const Fen<V>& fen) override {
        Board<Glinski> b{fen};
        return chooseRandom(b.getLegalMoves(b.mover()));
    }
};

class RandomAggressivePlayer : public Player {
public:
    RandomAggressivePlayer() {}
    ~RandomAggressivePlayer() override {}

    const std::string name() override { return "RandomAggressivePlayer"; }

    const Move& getNextMove(const Fen<V>& fen) override {
        Board<V> b{fen};
        const Moves& moves = b.getLegalMoves(b.mover());
        Moves captures{};
        std::copy_if(
            moves.cbegin(),
            moves.cend(),
            captures.begin(),
            [](const Move& m) { return m.isCapture(); }
            );
        if (captures.size() > 0) {
            return chooseRandom(captures);
        } else {
            return chooseRandom(moves);
        }
    }
};

PieceValue advancingMoveValuator(const Board<Glinski>& b, const Move& m);

class RandomAdvancingPlayer : public Player {
public:
    RandomAdvancingPlayer() {}
    ~RandomAdvancingPlayer() override {}

    const std::string name() override { return "RandomAdvancingPlayer"; }

    const Move& getNextMove(const Fen<V>& fen) override;
};

}  // namespace hexchess::core

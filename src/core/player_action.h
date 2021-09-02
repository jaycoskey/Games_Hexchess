// Games_Chess
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

#include "move.h"


namespace hexchess::core {

enum class PlayerActionEnum {
    Move,
    Offer_Draw,
    Offer_Draw_Accept,
    Offer_Draw_Decline,
    // Propose_Resignation        // Player suggests that opponent will lose
    // Propose_Resignation_Accept // Accept resignation proposed by opponent
    Resign
    // Timeout,               // TODO: Add once clocks are supported
    // Timeout_Claim, // Claim timeout based on clock not auto-enforced
};

class PlayerAction {
public:
    PlayerAction(PlayerActionEnum aenum)
        : _playerActionEnum{aenum},
          _optMove{std::nullopt}
    {
        assert(aenum != PlayerActionEnum::Move);
    }

    PlayerAction(const Move& move)
        : _playerActionEnum{PlayerActionEnum::Move},
          _optMove{std::make_optional(move)}
    { }

    PlayerActionEnum playerActionEnum() { return _playerActionEnum; }
    Move& move() { return _optMove.value(); }

private:
    PlayerActionEnum _playerActionEnum;
    OptMove _optMove;
};

} // namespace hexchess::core

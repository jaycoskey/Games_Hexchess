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

#include <string>

#include "player_action.h"


namespace hexchess::core {

const std::string player_action_enum_string(PlayerActionEnum pae) {
    static std::map<PlayerActionEnum, std::string> pae2s {
        { PlayerActionEnum::Move, "Move" },
        { PlayerActionEnum::Draw_Offer, "Draw-Offer" },
        { PlayerActionEnum::Draw_Accept, "Draw-Accept" },
        { PlayerActionEnum::Draw_Decline, "Draw-Decline" },
        { PlayerActionEnum::Resign, "Resign" }
    };
    return pae2s.at(pae);
}

}  //  namespace hexchess::core

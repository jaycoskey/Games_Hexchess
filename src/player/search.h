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
#include "move.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::player {

using core::Board;
using core::Color;
using core::Glinski;
using core::Move;
using core::Short;
using core::Value;

using core::negInfinity;
using core::posInfinity;

constexpr Short minSearchDepth = 3;

/// \brief Alpha-beta pruning with quiescent search.
///
/// Traditionally, a board scoring function is chosen so that
///     higher values reflect better choices for White,
///     while lower scores reflect a better score for Black.
/// So White is the maximizing player, while Black is the minimizing one.
///     \p alpha is the minimum score that the maximizing player (White) can get.
///     \p beta is the maximum score that the minimizing player (Black) can get.
///
/// \todo Consider using iterative deeping with cached evaluations.
std::pair<std::optional<Move>, Value> searchAlphaBeta(
    Board<Glinski>& b,
    Color mover,
    Short depthRemaining,
    Value alpha=negInfinity,
    Value beta=posInfinity,
    bool useQuiescentSearch=true,
    Short nonQuiescentDepthAdded=0);

}  // namespace hexchess::player

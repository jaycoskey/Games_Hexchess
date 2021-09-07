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

#include <algorithm>
#include <memory>

#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "search.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::player {

using hexchess::mkPair;

using core::Board;
using core::Color;
using core::Glinski;
using core::Move;
using core::OptMove;
using core::Short;
using core::Value;

using evaluation::Evaluation;

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
    Value alpha,
    Value beta,
    bool useQuiescentSearch,
    Short nonQuiescentDepthAdded)
{
    constexpr Short maxNonQuiescentDepthAdded = 3;  // Avoid diving too deep

    if (depthRemaining == 0 || b.getIsGameOver()) {
        return mkPair(std::nullopt, Evaluation::value(b));
    }

    if (mover == Color::Black) {
        // Minimizing
        Value minVal = posInfinity;
        std::optional<Move> optBestMove = std::nullopt;
        for (Move& m : b.getLegalMoves(mover)) {
            b.moveExec(m);

            // Quiescent search
            if (useQuiescentSearch
                && depthRemaining == 1
                && nonQuiescentDepthAdded < maxNonQuiescentDepthAdded
                && (m.isCapture() || m.isCheck() || m.isPromotion()))
            {
                depthRemaining++;  // Ensure we look at least one ply further
                nonQuiescentDepthAdded++;
            }

            Value value = searchAlphaBeta(
                b,
                nextPlayer(mover),
                depthRemaining - 1,
                alpha, beta,
                useQuiescentSearch,
                nonQuiescentDepthAdded
                ).second;
            b.moveUndo(m);
            if (value < minVal) {
                minVal = value;
                optBestMove = std::make_optional(m);
            }
            beta = std::min(beta, value);
            if (alpha >= beta) {
                break;  // alpha cutoff; pruning min; maximizer will block
            }
        }
        return mkPair<OptMove, Value>(optBestMove, minVal);
    } else {
        // Maximizing
        OptMove optBestMove{};
        Value maxVal = negInfinity;
        for (Move& m : b.getLegalMoves(mover)) {
            b.moveExec(m);

            // Quiescent search
            if (useQuiescentSearch) {
                if (depthRemaining == 1
                    && nonQuiescentDepthAdded < maxNonQuiescentDepthAdded
                    && (m.isCapture() || m.isCheck() || m.isPromotion())) {
                    depthRemaining++;  // Ensure we look at least one ply further
                    nonQuiescentDepthAdded++;
                }
            }

            Value value = searchAlphaBeta(b, nextPlayer(mover),
                             depthRemaining - 1,
                             alpha, beta,
                             useQuiescentSearch,
                             nonQuiescentDepthAdded
                             ).second;
            b.moveUndo(m);
            if (value > maxVal) {
                maxVal = value;
                optBestMove = std::make_optional(m);
            }
            alpha = std::max(alpha, value);
            if (alpha >= beta) {
                break;  // beta cutoff; pruning max; minimizer will block
            }
        }
        return mkPair<Move, Value>(optBestMove.value(), maxVal);
    }
}

}  // namespace hexchess::player

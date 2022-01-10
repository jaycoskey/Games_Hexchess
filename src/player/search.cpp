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
#include <iostream>
#include <memory>
#include <string>

#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "search.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"

using std::cout;
using std::string;

namespace hexchess::player {

using hexchess::mkPair;

using core::Board;
using core::Color;
using core::Glinski;
using core::Move;
using core::OptMove;
using core::Scope;
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
    Scope scope{"search.cpp:searchAlphaBeta"};
    constexpr Short maxNonQuiescentDepthAdded = 3;  // Avoid diving too deep

    print(cout, scope(), "mover=", color_long_string(mover),
        ". Entering with depthRemaining=", depthRemaining,
        ", alpha=", alpha, ", beta=", beta, "\n");
    if (depthRemaining == 0 || b.getIsGameOver()) {
        Value v = Evaluation::value(b);
        if (b.getIsGameOver()) {
            print(cout, scope(), "mover=", color_long_string(mover),
                ", game in game tree is over: value is ", v, "\n");
        }
        return mkPair(std::nullopt, v);
    }

    if (mover == Color::Black) {
        // Minimizing
        Value minVal = posInfinity;
        std::optional<Move> optBestMove = std::nullopt;

        print(cout, scope(), "mover=", color_long_string(mover),
            "Count of legal moves=", b.getLegalMoves(mover).size(), "\n");
        for (const Move& m : b.getLegalMoves(mover)) {
            string indent(4 * b.currentCounter(), ' ');
            print(cout, scope(), "(Minimizing) Mover=", color_long_string(mover),
                indent,
                ", counter=", b.currentCounter(),
                ". Evaluating sub-move=", m.move_pgn_string(false), "\n");
            b.moveExec(m);
            b.setLegalMoveCheckEnums(b.mover());  // Compute legal moves & check enums

#ifdef QUIESCENT_SEARCH
            // Quiescent search
            if (useQuiescentSearch
                && depthRemaining == 1
                && nonQuiescentDepthAdded < maxNonQuiescentDepthAdded
                && (m.isCapture() || m.isPromotion() || m.isCheck()))
            {
                depthRemaining++;  // Ensure we look at least one ply further
                nonQuiescentDepthAdded++;
            }
#endif  // QUIESCENT_SEARCH

            Value value = searchAlphaBeta(
                b,
                nextPlayer(mover),
                depthRemaining - 1,
                alpha, beta,
                useQuiescentSearch,
                nonQuiescentDepthAdded
                ).second;
            print(cout, scope(), "(Minimizing) Mover=", color_long_string(mover),
                indent,
                ", counter=", b.currentCounter(),
                ". Undoing move ", m.move_pgn_string(false), "\n");
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
        print(cout, scope(), "mover=", color_long_string(mover),
            ", returning with move=", optBestMove.value().move_pgn_string(true),
            ", value=", minVal, "\n");
        return mkPair<OptMove, Value>(optBestMove, minVal);
    } else {
        // Maximizing
        OptMove optBestMove{};
        Value maxVal = negInfinity;

        print(cout, scope(), "mover=", color_long_string(mover),
            "Count of legal moves=", b.getLegalMoves(mover).size(), "\n");
        for (const Move& m : b.getLegalMoves(mover)) {
            string indent(4 * b.currentCounter(), ' ');
            print(cout, scope(), "(Maximizing) Mover=", color_long_string(mover),
                indent,
                ", counter=", b.currentCounter(),
                ". Evaluating sub-move=", m.move_pgn_string(false), "\n");
            b.moveExec(m);
            b.getLegalMoves(b.mover());
            b.setLegalMoveCheckEnums(b.mover());  // Find legal moves & check enums

#ifdef QUIESCENT_SEARCH
            // Quiescent search
            if (useQuiescentSearch) {
                if (depthRemaining == 1
                    && nonQuiescentDepthAdded < maxNonQuiescentDepthAdded
                    && (m.isCapture() || m.isPromotion() || m.isCheck()))
                {
                    depthRemaining++;  // Ensure we look at least one ply further
                    nonQuiescentDepthAdded++;
                }
            }
#endif  // QUIESCENT_SEARCH

            Value value = searchAlphaBeta(b, nextPlayer(mover),
                             depthRemaining - 1,
                             alpha, beta,
                             useQuiescentSearch,
                             nonQuiescentDepthAdded
                             ).second;
            print(cout, scope(), "(Maximizing) Mover=", color_long_string(mover),
                indent,
                ", counter=", b.currentCounter(),
                ". Undoing move ", m.move_pgn_string(false), "\n");
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
        print(cout, scope(), "mover=", color_long_string(mover),
            ", returning with move=", optBestMove.value().move_pgn_string(true),
            ", value=", maxVal, "\n");
        return mkPair<Move, Value>(optBestMove.value(), maxVal);
    }
}

}  // namespace hexchess::player

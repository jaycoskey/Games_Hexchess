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

#include "board.h"
#include "move.h"
#include "player.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::core {

PieceValue advancingMoveValuator(const Board<Glinski>& b, const Move& m) {
   PieceValue result = 0;

    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        OptColorPieceType& ocpt = b.piecesSparse()[index];
        if (ocpt.has_value()) {
            auto [c, pt] = ocpt.value();
            if (c == b.mover()) {
                result += Glinski::rowIncreasingForward(index, c);
            }
        }
    }
    return result;
};

const Move& RandomAdvancingPlayer::getNextMove(const Fen<V>& fen) {
    Board<Glinski> b{fen};
    const Moves& moves = b.getLegalMoves(b.mover());

    // auto valuator
    std::function<PieceValue(const Move&)> valuator
        = [&b](const Move& m) -> PieceValue
          { return advancingMoveValuator(b, m); };
    const Moves& bestMoves = maxValues(moves, valuator);
    return chooseRandom(bestMoves);
}

}  // namespace hexchess::core

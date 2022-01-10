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

#include <iostream>
#include <map>

#include "board.h"
#include "evaluation.h"
#include "game_outcome.h"
#include "util_hexchess.h"
#include "variant.h"

using std::cout;

using std::map;

namespace hexchess::evaluation {

using core::Board;
using core::Color;
using core::GameOutcome;
using core::Glinski;
using core::PieceType;
using core::Scope;
using core::Value;

using core::negInfinity;
using core::posInfinity;


Value colorMult(Color c) {
    return c == Color::White ? 1 : -1;
}

/// Glinski: Q:10, R:5, B:3, N:4
/// Roczniak: Q6.660, R:4.460, B:2.6, N:2.3
const map<PieceType, Value> Evaluation::_pieceTypeToValue {
    {PieceType::King,  1'000'000},
    {PieceType::Queen,     6'660},
    {PieceType::Rook,      4'460},
    {PieceType::Bishop,    2'600},
    {PieceType::Knight,    2'300},
    {PieceType::Pawn,      1'000}
};

Value Evaluation::value_pieceTypes(const Board<Glinski>& b) {
    Scope scope{"Evaluation::value_pieceTypes"};

    Value result = 0;
    for (auto [from, c, pt] : b.piecesDense()) {
        // print(cout, scope(), "c=", c, ", pt=", pt, "\n");
        result += colorMult(c) * _pieceTypeToValue.at(pt);
    }
    return result;
}

// \todo: Chess evaluation starter pack:
// \todo: King danger:      Count of cells adjacent to King that are attacked.
// \todo: King mobility:    Count of legal moves
// \todo: Queen mobility:   Count of pseudo-legal moves
// \todo: Rook mobility:    Count of pseudo-legal moves
// \todo: Bishop mobility:  Count of pseudo-legal moves
// \todo: Bishop multiples: Bonus for 2 or three bishops
//     Note: Larry Kaufman suggests 0.5 Pawns for a Bishop pair in orthochess
// \todo: Knight mobility:  Count of pseudo-legal moves
// \todo: Pawn progress:    Number of spaces to promotion
// \todo: Pawn structure:   Pawns in connected columns vs isolated columns
// \todo: Pawn structure:   Stacked Pawns (i.e., same column)
// \todo: Lower priority: forks, pins, skewers, control of board center, etc.
Value Evaluation::value(const Board<Glinski>& b) {
    if (b.getIsGameOver()) {
        const GameOutcome& outcome = b.getOutcome();
        auto val = 1000 * (outcome.score(Color::White) - outcome.score(Color::Black));
        return static_cast<int>(val);
    }
    Value result = 0;
    result += value_pieceTypes(b);
    // result += getBishopCountBonus(Color::White) - getBishopCountBonus(Color::Black);
    return result;
}

}  // namespace hexchess::evaluation

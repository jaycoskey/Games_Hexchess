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

#include <map>

#include "board.h"
#include "util_hexchess.h"


namespace hexchess::evaluation {

using core::Board;
using core::Color;
using core::Glinski;
using core::PieceType;
using core::Value;

/// \todo Use instance and caching between calls
class Evaluation {
public:
    static Value value(const Board<Glinski>& b);
    static Value value_pieceTypes(const Board<Glinski>& b, Color c);
    static Value value_pieceTypes(const Board<Glinski>& b);

private:
    static const std::map<PieceType, Value> _pieceTypeToValue;
};

}  // namespace hexchess::evaluation

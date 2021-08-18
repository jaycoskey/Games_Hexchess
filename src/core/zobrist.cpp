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
#include <random>

#include "variant.h"
#include "zobrist.h"


using std::cout;


namespace hexchess::core {

ZHash randomBitstring() {
    static std::random_device dev{"/dev/random"};
    static std::mt19937_64 prng{dev()};
    return prng();
}

// Returns the index into a one-dimensional Zobrist table of size HASH_COUNT.
template<>
ZIndex Zobrist<Glinski>::getZIndex(Index index, Color c, PieceType pt) {
    typedef Glinski V;

    static std::map<Color, ZIndex> c2zi{
        {Color::Black, 0},
        {Color::White, 1}
        };
    static std::map<PieceType, ZIndex> pt2zi{
        {PieceType::King,   0}, {PieceType::Queen,  1},
        {PieceType::Rook,   2}, {PieceType::Bishop, 3},
        {PieceType::Knight, 4}, {PieceType::Pawn,   5}
        };

    ZIndex result = index * V::COLOR_COUNT * V::PIECE_TYPE_COUNT
                + c2zi.at(c) * V::PIECE_TYPE_COUNT
                + pt2zi.at(pt);
    return result;
}

extern template Zobrist<Glinski>::ZTable Zobrist<Glinski>::_zobristTable;

template<>
ZHash Zobrist<Glinski>::getZHash(ZIndex zIndex) {
    return _zobristTable[zIndex];
}

template<>
ZHash Zobrist<Glinski>::getZHash(Index index, Color c, PieceType pt) {
    ZIndex zIndex = getZIndex(index, c, pt);
    return _zobristTable[zIndex];
}

}  // namespace hexchess::core

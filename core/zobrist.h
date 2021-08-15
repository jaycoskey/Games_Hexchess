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

// For background on Zobrist hashing, see Wikipedia.

#include <cassert>
#include <cstddef>

#include "util_hexchess.h"


namespace hexchess::core {

using ZHash = std::size_t;
using ZIndex = int;

ZHash randomBitstring();

/// \brief Support for Zobrist hashing of the Board, to track the number of board layout repetitions.
template <typename Variant>
class Zobrist {
public:
    typedef Variant V;

    /// \brief The size of the Zobrist hash table needed for this variant.
    ///
    /// Note: Some Zobrist hashing scheme make use of en passant conditions.
    ///     This is not done here, since they are only created by Pawn moves, which
    ///     reset the Non-progress counter of half-moves without a capture of Pawn move.
    static constexpr Size HASH_COUNT = V::CELL_COUNT * V::COLOR_COUNT * V::PIECE_TYPE_COUNT;

    using ZTable = std::array<ZHash, HASH_COUNT>;

    /// \brief Returns the specified value in the Zobrist hash table.
    static ZHash getZHash(ZIndex zIndex);

    /// \brief Returns the Zobrist hash value for the specified
    ///        Index \p index, Color \p c, and PieceType \p pt.
    static ZHash getZHash(Index index, Color c, PieceType pt);

    /// \brief Returns the appropriate index into the (one-dimensional) Zobrist hash table.
    static ZIndex getZIndex(Index index, Color c, PieceType pt);
private:
    static ZTable _zobristTable;
};

}  // namespace hexchess::core

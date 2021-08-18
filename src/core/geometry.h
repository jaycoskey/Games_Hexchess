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

#include <vector>

#include "util.h"
#include "util_hexchess.h"


namespace hexchess::core {

struct HexDir;
using HexDirs = std::vector<HexDir>;

/// \brief A vector in hexagonal coordinates.
struct HexDir {
    HexDir(HexCoord h0, HexCoord h1) : hex0{h0}, hex1{h1} {}
    HexDir(const HexDir& other) = default;
    ~HexDir() {};
    HexDir& operator=(const HexDir& other) = delete;

    HexCoord hex0;
    HexCoord hex1;
};

using HexDirs = std::vector<HexDir>;

struct BoardDir {
    // Orthogonal directions
    static const HexDir NE;    ///< \brief Board direction: Northeast
    static const HexDir N;     ///< \brief Board direction: North
    static const HexDir NW;    ///< \brief Board direction: Northwest
    static const HexDir SW;    ///< \brief Board direction: Southwest
    static const HexDir S;     ///< \brief Board direction: South
    static const HexDir SE;    ///< \brief Board direction: Southeast

    // Diagonal directions
    static const HexDir E;     ///< \brief Board direction: East
    static const HexDir NNE;   ///< \brief Board direction: North-Northeast
    static const HexDir NNW;   ///< \brief Board direction: North-Northwest
    static const HexDir W;     ///< \brief Board direction: West
    static const HexDir SSW;   ///< \brief Board direction: South-Southwest
    static const HexDir SSE;   ///< \brief Board direction: South-Southeast

    static const HexDirs orthoDirs; ///< \brief The six "orthogonal" directions perpendicular to Cell edges
    static const HexDirs diagDirs;  ///< \brief The six "diagonal" directions going from a Cell center to its vertices
    static const HexDirs allDirs;   ///< \brief All "orthogonal" and "diagonal" directions

    static const HexDirs knightLeapDirs;  // \brief The 12 directions that a Knight can leap in hexagonal coordinates
};

/// \brief A point in hexagonal coordinates.
///
/// (The origin and directions of hex coordinates vary by Variant.
/// In Glinski chess, the origin lies at the Board's SW corner,
///     axis hex0 is incremented toward the SE,
///     and axis hex1 is incremented toward the N.)
struct HexPos {
    HexPos(HexCoord h0, HexCoord h1) : hex0{h0}, hex1{h1} {}
    HexPos(const HexPos& other) = delete;
    ~HexPos() {};
    HexPos& operator=(const HexPos& other) = delete;

    HexCoord hex0;
    HexCoord hex1;
};

/// \brief Addition of HexDir \p d to HexPos \p pos.
HexPos& operator+=(HexPos& pos, const HexDir& d);

/// \brief Addition of two HexDir objects.
HexDir& operator+=(HexDir& dir, const HexDir& other);

/// \brief Addition of HexDir \p d to HexPos \p pos.
HexPos operator+(const HexPos& pos, const HexDir& d);

/// \brief Addition of two HexDir objects.
HexDir operator+(const HexDir& d1, const HexDir& d2);

}  // namespace hexchess::core

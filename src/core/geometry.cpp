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
#include <vector>

#include "geometry.h"
#include "util.h"

using std::map;
using std::vector;


namespace hexchess::core {

HexDir::HexDir(HexCoord h0, HexCoord h1)
    : hex0{h0}, hex1{h1}
{
    assert(h0 != 0 || h1 != 0);
}

HexDir operator+(const HexDir& d1, const HexDir& d2) {
    return HexDir{d1.hex0 + d2.hex0, d1.hex1 + d2.hex1};
}

HexDir& operator+=(HexDir& dir, const HexDir& other) {
    dir.hex0 += other.hex0;
    dir.hex1 += other.hex1;
    return dir;
}

std::ostream& operator<<(std::ostream& os, const HexDir& d) {
    os << "(" << d.hex0 << ", " << d.hex1 << ")"; return os;
}

const HexDir BoardDir::NE { 1,  1};
const HexDir BoardDir::N  { 0,  1};
const HexDir BoardDir::NW {-1,  0};
const HexDir BoardDir::SW {-1, -1};
const HexDir BoardDir::S  { 0, -1};
const HexDir BoardDir::SE { 1,  0};

const HexDirs BoardDir::orthoDirs { NE, N, NW, SW, S, SE };

const HexDir BoardDir::E   { 2,  1};
const HexDir BoardDir::NNE { 1,  2};
const HexDir BoardDir::NNW {-1,  1};
const HexDir BoardDir::W   {-2, -1};
const HexDir BoardDir::SSW {-1, -2};
const HexDir BoardDir::SSE { 1, -1};

const HexDirs BoardDir::diagDirs { E, NNE, NNW, W, SSW, SSE };

const HexDirs BoardDir::allDirs {
    NE,  N,   NW,  SW,  S,   SE,
    E,   NNE, NNW, W,   SSW, SSE
};

const HexDirs BoardDir::knightLeapDirs {
    HexDir( 3,  2),
    HexDir( 2,  3),
    HexDir( 1,  3),
    HexDir(-1,  2),
    HexDir(-2,  1),
    HexDir(-3, -1),

    HexDir(-3, -2),
    HexDir(-2, -3),
    HexDir(-1, -3),
    HexDir( 1, -2),
    HexDir( 2, -1),
    HexDir( 3,  1)
};

HexDir operator-(const HexPos& p1, const HexPos& p2) {
    return HexDir{p1.hex0 - p2.hex0, p1.hex1 - p2.hex1};
}

HexPos operator+(const HexPos& pos, const HexDir& d) {
    return HexPos{pos.hex0 + d.hex0, pos.hex1 + d.hex1};
}

HexPos& operator+=(HexPos& pos, const HexDir& d) {
    pos.hex0 += d.hex0;
    pos.hex1 += d.hex1;
    return pos;
}

std::ostream& operator<<(std::ostream& os, const HexPos& d) {
    os << "(" << d.hex0 << ", " << d.hex1 << ")";
    return os;
}

}  // namespace hexchess::core

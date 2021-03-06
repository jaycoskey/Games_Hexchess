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
#include <string>
#include <vector>

#include "geometry.h"
#include "variant.h"

using std::cout;
using std::string;

using std::map;
using std::pair;
using std::vector;


namespace hexchess::core {

template<>
HexRay<Glinski>::HexRay(Index start, const HexDir& dir)
    : _start{start}, _dir{dir}, _indices{}
{
    for (HexPos cursor = V::indexToPos(start) + dir;
             V::isOnBoard(cursor);
             cursor += dir)
    {
        Index curIndex = V::posToIndex(cursor);
        _indices.push_back(curIndex);
    }
}

// ========================================
// Board coordinates

HexCoord Glinski::hex0(Index index) {
    static const std::vector<HexCoord> i2h0 {
                      0,  1,  2,  3,  4,  5,
                    0,  1,  2,  3,  4,  5,  6,
                  0,  1,  2,  3,  4,  5,  6,  7,
                0,  1,  2,  3,  4,  5,  6,  7,  8,
              0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
              1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                2,  3,  4,  5,  6,  7,  8,  9,  10,
                  3,  4,  5,  6,  7,  8,  9,  10,
                    4,  5,  6,  7,  8,  9,  10,
                      5,  6,  7,  8,  9,  10
        };
    return i2h0[index];
}

HexCoord Glinski::hex1(Index index)
{
    static const std::vector<HexCoord> i2h1 {
                  0,  0,  0,  0,  0,  0,
                1,  1,  1,  1,  1,  1,  1,
              2,  2,  2,  2,  2,  2,  2,  2,
            3,  3,  3,  3,  3,  3,  3,  3,  3,
          4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
        5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
          6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
            7,  7,  7,  7,  7,  7,  7,  7,  7,
              8,  8,  8,  8,  8,  8,  8,  8,
                9,  9,  9,  9,  9,  9,  9,
                  10, 10, 10, 10, 10, 10
        };
    return i2h1[index];
}

bool Glinski::isOnBoard(HexCoord hex0, HexCoord hex1) {
    bool result =
           hex0 >= 0          // W
        && hex0 <= 10         // E
        && hex1 >= 0          // SW
        && hex1 <= 10         // NE
        && hex1 - hex0 <= 5   // NW
        && hex0 - hex1 <= 5;  // SE
    return result;
}

const Strings Glinski::cellNames {
                   "A1", "B1", "C1", "D1", "E1", "F1",
                "A2", "B2", "C2", "D2", "E2", "F2", "G1",
             "A3", "B3", "C3", "D3", "E3", "F3", "G2", "H1",
          "A4", "B4", "C4", "D4", "E4", "F4", "G3", "H2", "I1",
       "A5", "B5", "C5", "D5", "E5", "F5", "G4", "H3", "I2", "K1",
    "A6", "B6", "C6", "D6", "E6", "F6", "G5", "H4", "I3", "K2", "L1",
       "B7", "C7", "D7", "E7", "F7", "G6", "H5", "I4", "K3", "L2",
          "C8", "D8", "E8", "F8", "G7", "H6", "I5", "K4", "L3",
             "D9", "E9", "F9", "G8", "H7", "I6", "K5", "L4",
                "E10","F10","G9", "H8", "I7", "K6", "L5",
                   "F11","G10","H9", "I8", "K7", "L6"
};

const string& Glinski::cellName(Index index) {
    return cellNames.at(index);
}

Index Glinski::cellNameToIndex(const string& name) {
    static const map<string, Index> n2i { []()
        {
            map<string, Index> result;
            for (Index index = 0; index < V::CELL_COUNT; ++index) {
                string name = V::cellNames[index];
                result[name] = index;
            }
            return result;
        }()};
    return n2i.at(name);
}

// ========================================
// Piece placement

const string Glinski::fenInitial{"b/qk/nbn/r2r/p1b1p/1p2p1/1p1p1/2pp2/2p2/6/5/6/2P2/2PP2/1P1P1/1P2P1/P1B1P/R2R/NBN/QK/B w - - 1 1"};

const Index Glinski::fenOrderToIndex[Glinski::CELL_COUNT] {
                       85,
                    78,   86,
                 70,   79,   87,
              61,   71,   80,   88,
           51,   62,   72,   81,   89,
        40,   52,   63,   73,   82,   90,
           41,   53,   64,   74,   83,
        30,   42,   54,   65,   75,   84,
           31,   43,   55,   66,   76,
        21,   32,   44,   56,   67,   77,
           22,   33,   45,   57,   68,
        13,   23,   34,   46,   58,   69,
           14,   24,   35,   47,   59,
        6,    15,   25,   36,   48,   60,
           7,    16,   26,   37,   49,
        0,    8,    17,   27,   38,   50,
           1,    9,    18,   28,   39,
              2,    10,   19,   29,
                 3,    11,   20,
                    4,    12,
                       5
        };

const Short Glinski::fenRowLengths[Glinski::ROW_COUNT] {
    1,2,3,4,5,6,  5,6,5,6,  5,  6,5,6,5,  6,5,4,3,2,1
};

// ========================================
// Initialization of private data

const map<pair<HexCoord, HexCoord>, Index> Glinski::_hexToIndex = []() {
    map<pair<HexCoord, HexCoord>, Index> result{};

    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        HexCoord hex0 = Glinski::hex0(index);
        HexCoord hex1 = Glinski::hex1(index);
        result[mkPair(hex0, hex1)] = index;
    }
    return result;
}();

const Indices Glinski::getLeapDests(Index index, const HexDirs& dirs) {
    Indices result{};

    for (const HexDir& dir : dirs) {
        HexPos destPos = Glinski::indexToPos(index) + dir;
        if (isOnBoard(destPos)) {
            Index destInd = posToIndex(destPos);
            result.push_back(destInd);
        }
    }
    return result;
}

const HexRays<Glinski> Glinski::getSlideRays(Index index, const HexDirs& dirs) {
    HexRays<V> result{};

    for (const HexDir& dir : dirs) {
        HexRay<V> ray{index, dir};
        if (!ray.isEmpty()) {
            result.push_back(ray);
        }
    }
    return result;
}

const vector<Indices> Glinski::kingDests { []()
    {
        vector<Indices> result;
        for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
            result.push_back(getLeapDests(index, Glinski::kingLeapDirs));
        }
        return result;
    }()
};

const vector<HexRays<Glinski>> Glinski::queenRays { []()
    {
        vector<HexRays<Glinski>> result;
        for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
            HexRays<Glinski> rays = getSlideRays(index, Glinski::queenSlideDirs);
            result.push_back(getSlideRays(index, Glinski::queenSlideDirs));
        }
        return result;
    }()
};

const vector<HexRays<Glinski>> Glinski::rookRays { []()
    {
        vector<HexRays<Glinski>> result;
        for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
            result.push_back(getSlideRays(index, Glinski::rookSlideDirs));
        }
        return result;
    }()
};

const vector<HexRays<Glinski>> Glinski::bishopRays { []()
    {
        vector<HexRays<Glinski>> result{};
        for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
            result.push_back(getSlideRays(index, Glinski::bishopSlideDirs));
        }
        return result;
    }()
};

const vector<Indices> Glinski::knightDests { []()
    {
        vector<Indices> result{};
        for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
            result.push_back(getLeapDests(index, Glinski::knightLeapDirs));
        }
        return result;
    }()
};

const map<Color, map<Index, Indices>> Glinski::colorToPawnAdvance1Indices = []() {
    map<Color, map<Index, Indices>> result{};

    for (Color c : {Color::Black, Color::White}) {
        result[c] = map<Index, Indices>{};
        for (Index from = 0; from < Glinski::CELL_COUNT; ++from) {
            HexPos curPos = Glinski::indexToPos(from);
            HexPos fwd1 = curPos + pawnAdvanceDirs(c)[0];
            if (isOnBoard(fwd1)) { result[c][from].push_back(Glinski::posToIndex(fwd1)); }
        }
    }
    return result;
}();

// Only use the Pawn's home positions as initial positions.
// Note: We do not check here to see if the space one ahead of the Pawn is blocked.
//     That is taken care of by the user of this data.
const map<Color, map<Index, Indices>> Glinski::colorToPawnAdvance2Indices = []() {
    map<Color, map<Index, Indices>> result{};

    for (Color c : {Color::Black, Color::White}) {
        for (Index from : (c == Color::Black ? _bp_indices : _wp_indices)) {
            HexPos curPos = Glinski::indexToPos(from);
            for (const HexDir& fdir : V::pawnAdvanceDirs(c)) {
                HexPos fwd2 = curPos + fdir + fdir;
                if (V::isOnBoard(fwd2)) {
                    result[c][from].push_back(Glinski::posToIndex(fwd2));
                }
            }
        }
    }
    return result;
}();

const map<Color, map<Index, Indices>> Glinski::colorToPawnCaptureIndices = []() {
    map<Color, map<Index, Indices>> result{};

    for (Color c : {Color::Black, Color::White}) {
        result[c] = map<Index, Indices>{};
        for (Index from = 0; from < Glinski::CELL_COUNT; ++from) {
            result[c][from] = Indices{};
            for (HexDir dir : V::pawnCaptureDirs(c)) {
                HexPos destPos = indexToPos(from) + dir;
                if (V::isOnBoard(destPos)) {
                    result[c][from].push_back(posToIndex(destPos));
                }
            }
        }
    }
    return result;
}();

const map<Color, map<Index, Glinski::Bits>> Glinski::colorToPawnCaptureBits = []() {
    map<Color, map<Index, Glinski::Bits>> result{};

    for (Color c : {Color::Black, Color::White}) {
        result[c] = map<Index, Glinski::Bits>{};
        for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
            result[c][index] = Glinski::Bits{};
            for (HexDir dir : V::pawnCaptureDirs(c)) {
                HexPos destPos = V::indexToPos(index) + dir;
                if (V::isOnBoard(destPos)) {
                    result[c][index].set(posToIndex(destPos));
                }
            }
        }
    }
    return result;
}();

const map<Color, typename Glinski::Bits> Glinski::colorToPawnPromotionBits = []() {
    map<Color, typename Glinski::Bits> result{};

    result[Color::Black] = Glinski::Bits();
    result[Color::White] = Glinski::Bits();

    std::vector<int> bp_promo{0, 1, 2, 3, 4, 5, 12, 20, 29, 39, 50};
    for (Index k : bp_promo) {
        result[Color::Black][k] = true;
    }
    std::vector<int> wp_promo{40, 51, 61, 70, 78, 85, 86, 87, 88, 89, 90};
    for (Index k : wp_promo) {
        result[Color::White][k] = true;
    }
    return result;
}();

const map<Color, typename Glinski::Bits> Glinski::colorToPawnStartBits = []() {
    map<Color, Glinski::Bits> result{};
    result[Color::Black] = Glinski::Bits();
    result[Color::White] = Glinski::Bits();
    for (int index : Glinski::_bp_indices) {
        result[Color::Black][index] = true;
    }
    for (int index : Glinski::_wp_indices) {
        result[Color::White][index] = true;
    }
    return result;
}();

const HexDirs& Glinski::pawnAdvanceDirs(Color c) {
    static const HexDirs bForwards{ HexDir{0, -1} };
    static const HexDirs wForwards{ HexDir{0,  1} };
    return c == Color::Black ? bForwards : wForwards;
}

const HexDirs& Glinski::pawnCaptureDirs(Color c) {
    static const HexDirs bForwards{ HexDir(-1, -1), HexDir( 1, 0) };
    static const HexDirs wForwards{ HexDir(-1,  0), HexDir( 1, 1) };
    return c == Color::Black ? bForwards : wForwards;
}

const PieceTypes Glinski::promotionPieceTypes {
    PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight
};

// ========================================
// Castling

// const vector<Castling<Glinski>>& Glinski::castlings()
// {
//     static const vector<Castling<Glinski>> result{};
//     return result;
// }

}  // namespace hexchess::core

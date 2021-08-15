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

#include <iomanip>
#include <iostream>

#include "util_hexchess.h"
#include "variant.h"
#include "zobrist.h"


using std::cout;
using std::string;

using hexchess::core::Color;
using hexchess::core::Glinski;
using hexchess::core::Index;
using hexchess::core::PiecePlacement;
using hexchess::core::PieceType;
using hexchess::core::ZHash;
using hexchess::core::ZIndex;
using hexchess::core::Zobrist;

using hexchess::core::randomBitstring;


void printHexValue(ZHash zh, string end="\n") {
    cout << "0x"
         << std::hex
         << std::setfill('0') << std::setw(16) << zh
         << std::dec
         << end;
}

/// \brief Historical: This was used to populate the Zobrist hash table.
void printRandomBitstrings() {
    for (int k = 0; k < Zobrist<Glinski>::HASH_COUNT; ++k) {
        printHexValue(randomBitstring());
    }
}

void printZobristPieceHash(Index index, Color c, PieceType pt, ZIndex zi, ZHash zh) {
    cout << "  Zobrist hash of " << c << " " << pt << " @ " << index
         << " = _zobristTable["
         << std::setw(4) << zi << std::setw(2)
         << "] = ";
    printHexValue(zh);
}

void _test_zobrist_color_piecetype(
    Index index, Color c, PieceType pt,
    string description,
    ZHash expectedZHash=0,
    bool verbose=false)
{
    typedef Zobrist<Glinski> ZG;

    ZIndex zi = ZG::getZIndex(index, c, pt);
    ZHash zh = ZG::getZHash(zi);
    if (verbose) {
        cout << description << ":\n";
        printZobristPieceHash(index, c, pt, zi, zh);
    }
    if (expectedZHash > 0) {
        assert(zh == expectedZHash);
    }
}

/// \brief Test: A Black King at initial position has the expected Zobrist hash.
void test_zobrist_initial_bk(bool verbose=false) {
    _test_zobrist_color_piecetype(86, Color::Black, PieceType::King,
        "Black King at initial position (G10)",
        0x5dacf7eba7922c5f,
        verbose
        );
}

/// \brief Test: A White Queen at initial position has the expected Zobrist hash.
void test_zobrist_initial_wq(bool verbose=false) {
    _test_zobrist_color_piecetype(4, Color::White, PieceType::Queen,
        "White King at initial position (E0)",
        0x66ea8004a7e610e5,
        verbose
        );
}

/// \brief Test: A Board with initial layout has the expected Zobrist hash.
void test_zobrist_initial_board(bool verbose=false) {
    ZHash boardHash = 0;

    for (const PiecePlacement& pp : Glinski::piecePlacements()) {
        const auto [index, c, pt] = pp;
        boardHash ^= Zobrist<Glinski>::getZHash(index, c, pt);
    }
    if (verbose) {
        cout << "Initial board hash = ";
        printHexValue(boardHash);
        cout << "\n";
    }
    assert(boardHash == 0x712bf5ea63571cf5);
}

int main(int argc, char *argv[]) {
    test_zobrist_initial_bk();
    test_zobrist_initial_wq();
    test_zobrist_initial_board();
}

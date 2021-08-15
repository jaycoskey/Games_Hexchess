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

#include <cassert>
#include <cctype>

#include <iostream>
#include <map>
#include <string>

#include "util.h"
#include "util_hexchess.h"


using std::string;


namespace hexchess::core {

template<typename Variant>
typename Variant::Bits indexToBits(Index index) {
    return Variant::Bits(1 << index);
}

const string color_string(Color c) {
    return c == Color::Black ? "B" : "W";
}

const string piece_type_string(PieceType pt) {
    static std::map<PieceType, std::string> pt2s = {
        { PieceType::King,   "K" },
        { PieceType::Queen,  "Q" },
        { PieceType::Rook,   "R" },
        { PieceType::Bishop, "B" },
        { PieceType::Knight, "N" },
        { PieceType::Pawn,   "P" }
    };
    return pt2s[pt];
}

const string piece_string(Color c, PieceType pt) {
    string result = color_string(c) + piece_type_string(pt);
    assert(result.size() == 2);
    return result;
}

const string piece_unicode(Color c, PieceType pt, CellShade cs) {
    string result = string{};

    // Set background
    switch(cs) {
    case CellShade::Light:
        result += "\u001b[43m";  // Yellow
        break;
    case CellShade::Medium:
        result += "\u001b[42m";  // Green
        break;
    case CellShade::Dark:
        result += "\u001b[41m";  // Red
        break;
    default:
        throw std::logic_error("PieceType::to_unicode: Unrecognized cell color");
    }

    // Add chess piece character
    switch (pt) {
    case PieceType::King:
        result += c == Color::Black ? "\u265A" : "\u2654";
        break;
    case PieceType::Queen:
        result += c == Color::Black ? "\u265B" : "\u2655";
        break;
    case PieceType::Rook:
        result += c == Color::Black ? "\u265C" : "\u2656";
        break;
    case PieceType::Bishop:
        result += c == Color::Black ? "\u265D" : "\u2657";
        break;
    case PieceType::Knight:
        result += c == Color::Black ? "\u265E" : "\u2658";
        break;
    case PieceType::Pawn:
        result += c == Color::Black ? "\u265F" : "\u2659";
        break;
    default:
        throw std::logic_error("PieceType::to_ascii: Unrecognized piece type");
    }

    // Reset background
    result += "\u001b[0m";

    return result;
}

/// \brief \todo Refactor fen_string(Color, PieceType) to avoid narrowing conversion
const string fen_string(Color c, PieceType pt) {
    string result = piece_type_string(pt);
    return c == Color::Black ? string{static_cast<char>(std::tolower(result[0]))} : result;
}

}  // namespace hexchess::core

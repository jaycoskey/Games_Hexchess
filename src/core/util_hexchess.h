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

#include <math.h>

#include <array>
#include <bitset>
#include <iostream>
#include <map>
#include <optional>
#include <ostream>
#include <vector>

#include <QObject>


namespace hexchess {
    extern bool events_verbose;
}

namespace hexchess::core {
Q_NAMESPACE

// ========================================
// Fundamental types
// ========================================

/// \brief The index of a board location in a std::bitset or other collection of cells.
using Index = int;
using OptIndex = std::optional<Index>;

/// \brief A collection of indices, such as the possible destinations cells of a given Knight.
using Indices = std::vector<Index>;

using Size = std::size_t;

/// \brief Any integer that can fit in 16 bits: Moves in a game, size of Zobrish hash table, etc.
using Short = int;
using Shorts = std::vector<Short>;

/// \brief One of the two coordinate values of a Cell location.
///
/// (Note: The layout of a Cell, based on its coordinates, is Variant-specific.)
using HexCoord = Short;

/// \brief A count of the number of times a board piece has been moved.
///
/// In the chess world, the term "half-moves" refers to what many would call "moves".
/// The type alias HalfMoveCounter informs the reader what is being referred to.
/// Instances of this type are given names that contain the string "counter".
/// For example, the variable that stores the history of "half-moves" counts without
/// a capture or Pawn move is called nonProgressCounters.
using HalfMoveCounter = Short;

using Score = double;  // For game scores, including 0.5-0.5

using Strings = std::vector<std::string>;

/// \brief A measure of value of a piece, or a board.
///        Currently expressed as millipawns.
using Value = int;
constexpr Value negInfinity{-1'000'000};
constexpr Value posInfinity{+1'000'000};

// ========================================
// Enums
// ========================================

/// \brief The cells on hexagonal chess boards usually have three shades: Light, Medium, and Dark.
///
/// On hexagonal chess boards, the center cell (when there is one) is customarily Medium.
/// Bishops always stay on cells of a given shade, whereas Knights change shade with every move.
/// Throughout this code base, the term "shade" always refers to the shade of a cell,
/// never to the Color of a Piece.
enum class CellShade {
    Light,
    Medium,
    Dark
};
inline const std::string cell_shade_string(CellShade cs) {
    static std::map<CellShade, std::string> cs2s = {
        { CellShade::Light,  "Light" },
        { CellShade::Medium, "Medium" },
        { CellShade::Dark,   "Dark" }
    };
    return cs2s[cs];
}
inline std::ostream& operator<<(std::ostream& os, CellShade cs) {
    os << cell_shade_string(cs);
    return os;
}

/// \brief Color refers to the two players in two-player chess variants, whether the pieces
///     used to show the board are Black and White, or some other colors.
enum class Color {
    Black,
    White
};
Q_ENUM_NS(Color)

using OptColor = std::optional<Color>;

inline Color opponent(Color c) { return c == Color::Black ? Color::White : Color::Black; }
inline Color nextPlayer(Color c) { return opponent(c); }
inline Color prevPlayer(Color c) { return opponent(c); }

const std::string color_long_string(Color c);
const std::string color_short_string(Color c);

inline std::ostream& operator<<(std::ostream& os, Color c) {
    os << color_short_string(c);
    return os;
}

/// \brief PieceType refers to the six piece types in standard chess.
///
/// Note that some (hexagonal) variants have fewer or different types.
/// For example, Mini Hexchess does not have Queens, and Wellisch chess
/// does not have bishops.
///
/// Novel piece types not included in standard chess are called "fairy"
/// piece types. For example, Glinski-Capablanca chess has a piece type
/// called a "Chancellor" (Rook + Knight compound) and an archbishop
/// (Bishop + Knight compound).
enum class PieceType {
    King, Queen, Rook, Bishop, Knight, Pawn
};
constexpr PieceType pieceTypes[6] {
    PieceType::King,
    PieceType::Queen,
    PieceType::Rook,
    PieceType::Bishop,
    PieceType::Knight,
    PieceType::Pawn
};
using PieceTypes = std::vector<PieceType>;  // For listing available promotion types
PieceType piece_type_parse(char ch);
const std::string piece_type_string(PieceType pt);

inline std::ostream& operator<<(std::ostream& os, PieceType pt) {
    os << piece_type_string(pt);
    return os;
}

/// \brief Returns a two-character, upper-case code for each piece, such as BK or WQ.
///
const std::pair<Color, PieceType> piece_fen_parse(char ch);
const std::string piece_fen_string(Color c, PieceType pt);  ///< \brief One character
const std::string piece_string(Color c, PieceType pt);      ///< \brief Two characters
const std::string piece_unicode(Color c, PieceType pt);     ///< \brief For printing icons in ASCII

enum class UiMode {
    Api,
    Graphics,
    Text
};

/// \brief The different variants of hexagonal chess. (Not all are supported.)
/// * Flat-top hexagons:
///   * Glinksi:      91-cell board. Roomy layout.   Pawns capture orthogonally. No castling.
///   * McCooey:      91-cell board. Compact layout. Pawns capture diagonally.   No castling.
///   * Shafran:      70-cell board. Flat layout.    Pawns capture diagonally.   Four types of castling.
///   * MiniHexchess: 37-cell board. Only 9 pieces per player. Pawns capture diagonally. No castling.
/// * Flat-side hexagons:
///   * Brusky: 84-cell irregularly-shaped board. Pieces in first two rows. Two types of castling.
///   * DeVasa: 81-cell board with 9 rows and 8 columns. Two types of castling.
enum class Variant {
    Glinksi,
    McCooey,
    Shafran,
    MiniHexchess,
    Brusky,
    DeVasa
};

/// \brief Records the different types of Castling supported by a variant.
///
/// Note that Glinski chess does not support Castling.
enum class CastlingEnum
{
    Kingside,          ///< Notation: 0-0
    Queenside,         ///< Notation: 0-0-0
    LongBishopside,    ///< Notation: B-0-0-0
    LongQueenside,     ///< Notation: Q-0-0-0
    ShortBishopside,   ///< Notation: B-0-0
    ShortQueenside     ///< Notation: Q-0-0
};

/// \brief The specifics on types of Castling, with the info needed
///        to detect move availability, and to execute the move.
template<typename Variant>
struct Castling {
    typedef Variant V;

    std::string code;

    Index kingFrom;
    Index kingTo;
    Index rookFrom;
    Index rookTo;
    typename V::Bits between;
};

}  // namespace hexchess::core

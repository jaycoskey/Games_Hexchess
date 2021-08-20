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

#include <map>
#include <vector>

#include "geometry.h"
#include "util.h"
#include "util_hexchess.h"


namespace hexchess::core {

using ColorPieceType = std::pair<Color, PieceType>;
using OptColorPieceType = std::optional<ColorPieceType>;

using PiecesDense = std::vector<std::tuple<Index, Color, PieceType>>;
using PiecesSparse = std::vector<OptColorPieceType>;

/// \brief A (possibly empty) ordered sequence of Indexes in given direction from
///        a reference cell. The sequence does not include the reference cell.
///
/// This is used to represent the possible destinations of a slider piece.
/// For example, a Queen at the center of the board can move in 12 directions,
/// but from a corner, some directions are blocked, and would be associated
/// with empty HexRays.
///
/// \todo Implement support for Castling when adding McCooey or Shafran variants.
template <typename Variant>
struct HexRay {
    typedef Variant V;

    HexRay(Index start, const HexDir& dir);
    ~HexRay() {};

    bool size() const { return indices.size(); }

    /// Returns true if there are no indices this HexRay.
    bool isEmpty() const { return indices.empty(); }

    typename V::Bits bits;     // For initial batch testing
    Indices indices;  // For iterating after batch test found obstruction
};

/// \brief A collection of HexRays, which can be used to represent possible
///        destinations of a slider piece from a specified start Cell.
template<typename Variant>
using HexRays = std::vector<HexRay<Variant>>;

/// \brief All the variant-specific information regarding board, pieces,
///        and other rules of play.
class Glinski {
public:
    typedef Glinski V;

    static constexpr Short CELL_COUNT = 91;       ///< \brief Number of cells (i.e., spaces) on the board
    static constexpr Short COLOR_COUNT = 2;       ///< \brief Number of Colors (i.e., Players)
    static constexpr Short PIECE_TYPE_COUNT = 6;  ///< \brief Types of pieces: King, Queen, Rook, etc.
    static constexpr Short ROW_COUNT = 21;        ///< \brief Rows in this Variant's board. Used in FEN.

    typedef std::bitset<CELL_COUNT> Bits;

    // ========================================
    // Board coordinates

    /// \brief String describing initial piece placement, and som eother game state info.
    static const std::string fenInitial;

    /// \brief Order of indices of the board's cells as they are traversed in FEN order
    ///        (left-to-right, then top-down).
    static const Index fenOrderToIndex[V::CELL_COUNT];

    /// \brief Number of rows. Used for reading and writing FEN strings.
    static const Short fenRowLengths[V::ROW_COUNT];

    static HexCoord hex0(Index index);  ///< \brief Returns the first Hex coordinate of the cell.
    static HexCoord hex1(Index index);  ///< \brief Returns the second Hex coordinate of the cell.
    static HexCoord column(Index index) { return hex0(index); }

    // \brief Returns row, where the midline is row #0, Black's home is row #10, and White's is -10
    static HexCoord row(Index index) { return 2 * hex1(index) - hex0(index) - 5; }
    static HexCoord rowIncreasingForward(Index index, Color c) {
        HexCoord multiplier = c == Color::Black ? -1 : 1;
        return multiplier * row(index);
    }

    /// \brief Convert between representaions of a Cell.
    static HexPos indexToPos(Index index) { return HexPos(hex0(index), hex1(index)); }
    /// \brief Convert between representaions of a Cell.
    static Index hexToIndex(HexCoord hex0, HexCoord hex1) { return _hexToIndex[mkPair(hex0, hex1)]; }
    /// \brief Convert between representaions of a Cell.
    static Index posToIndex(const HexPos& pos) { return hexToIndex(pos.hex0, pos.hex1); }

    /// \brief Returns true if the given hex coordinates are on the board.
    static bool isOnBoard(HexCoord hex0, HexCoord hex1);
    /// \brief Returns true if the given HexPos represents a Cell on the board.
    static bool isOnBoard(const HexPos& pos) { return isOnBoard(pos.hex0, pos.hex1); }

    /// \brief The standard names (e.g., A0, L6, etc.) of the Cells
    static const Strings cellNames;
    static const std::string& cellName(Index index);
    static Index nameToIndex(const std::string& name);

    // ========================================
    // Cell shading

    static CellShade cellShade(Index index) { return cellShade(hex0(index), hex1(index)); }
    static CellShade cellShade(HexCoord hex0, HexCoord hex1) { return _cellShades[(hex0 + hex1) % 3]; }
    static CellShade cellShade(const HexPos& pos) { return cellShade(pos.hex0, pos.hex1); }

    // ========================================
    // Piece Directions

    static const inline HexDirs& kingLeapDirs    = BoardDir::allDirs;
    static const inline HexDirs& queenSlideDirs  = BoardDir::allDirs;
    static const inline HexDirs& rookSlideDirs   = BoardDir::orthoDirs;
    static const inline HexDirs& bishopSlideDirs = BoardDir::diagDirs;
    static const inline HexDirs& knightLeapDirs  = BoardDir::knightLeapDirs;

    static const HexDir& pawnAdvanceDir(Color c);
    static const HexDirs& pawnCaptureDirs(Color c);

    // ========================================
    // Board locations

    static const V::Bits& pawnPromotionBits(Color c) { return colorToPawnPromotionBits[c]; }
    static const V::Bits& pawnStartBits(Color c) { return colorToPawnStartBits[c]; }

    // static constexpr int bk_indices[1] = {86};
    // static constexpr int bq_indices[1] = {78};
    // static constexpr int br_indices[2] = {61, 88};
    // static constexpr int bb_indices[3] = {72, 79, 85};
    // static constexpr int bn_indices[2] = {70, 87};
    // static constexpr int bp_indices[9] = {51, 52, 53, 54, 55, 65, 74, 82, 89};

    // static constexpr int wk_indices[1] = {12};
    // static constexpr int wq_indices[1] = {4};
    // static constexpr int wr_indices[2] = {2, 29};
    // static constexpr int wb_indices[3] = {5, 11, 18};
    // static constexpr int wn_indices[2] = {3, 20};
    // static constexpr int wp_indices[9] = {1, 8, 16, 25, 35, 36, 37, 38, 39};

    // ========================================
    // Piece movement lookup

    /// \todo Change method signature to flat-side variants w/ 2 advance dirs (e.g., Brusky, De Vasa)
    static const std::map<Index, Index> pawnAdvance1Indices(Color c) { return colorToPawnAdvance1Indices[c]; }

    /// \todo Change method signature to flat-side variants w/ 2 advance dirs (e.g., Brusky, De Vasa)
    static const std::map<Index, Index> pawnAdvance2Indices(Color c) { return colorToPawnAdvance2Indices[c]; }

    static const V::Bits pawnCaptureBits(Color c) { return colorToPawnCaptureBits[c]; }

    ///< \brief Represents all Cells a King can leap to, indexed by initial position.
    ///         This does not include Castling, which is handled separately.
    static std::vector<Indices>    kingDests;

    static std::vector<HexRays<V>> queenRays;    ///< \brief Represents all Cells that a Queen can slide to.
    static std::vector<HexRays<V>> rookRays;     ///< \brief Represents all Cells that a Rook can slide to.
    static std::vector<HexRays<V>> bishopRays;   ///< \brief Represents all Cells that a Bishop can slide to.
    static std::vector<Indices>    knightDests;  ///< \brief Represents all Cells that a Knight can leap to.

    /// \brief Represents start and destination Cells for single-step Pawn moves.
    static std::map<Color, std::map<Index, Index>> colorToPawnAdvance1Indices;

    /// \brief Represents start and destination Cells for double-step Pawn moves.
    static std::map<Color, std::map<Index, Index>> colorToPawnAdvance2Indices;

    /// \brief Represents start and destination Cells for Pawn capture moves.
    static std::map<Color, typename V::Bits> colorToPawnCaptureBits;

    /// \brief Static map (per Color) of which Cells lead to Pawn promotion.
    static std::map<Color, typename V::Bits> colorToPawnPromotionBits;

    /// \brief Static map (per Color) of which Cells are Pawn start locations.
    ///
    /// For each Pawn on its initial locations, colorToPawnAdvance2Indices
    /// can be used to look up destination cells.
    static std::map<Color, typename V::Bits> colorToPawnStartBits;

    // ========================================
    // Castling
    // static bool isCastlingSupported() { return false; }
    // static const std::vector<Castling<V>>& castlings();

private:
    // ========== Board geometry
    /// \brief Lookup map to convert from hex coordinates to Index
    static std::map<std::pair<HexCoord, HexCoord>, Index> _hexToIndex;

    // ========== Board shading
    /// \brief Array of CellShades, used to determine the shade of a cell from hex coordinates.
    static constexpr CellShade _cellShades[3]{CellShade::Light, CellShade::Medium, CellShade::Dark};

    // ========== Pawn placement
    /// \brief For initial Piece placement. Also to determine which Pawns can double-step.
    static constexpr int _bp_indices[9] = {51, 52, 53, 54, 55, 65, 74, 82, 89};
    /// \brief For initial Piece placement. Also to determine which Pawns can double-step.
    static constexpr int _wp_indices[9] = {1, 8, 16, 25, 35, 36, 37, 38, 39};

    // ========== Piece movement caching support methods
    /// \brief Cached info for determining pseudo-legal moves for "leaper" pieces: King, Knight.
    static Indices getLeapDests(Index index, const HexDirs& dirs);

    /// \brief Cached info for determining pseudo-legal moves for "slider" pieces: Queen, Rook, Bishop.
    static HexRays<V> getSlideRays(Index index, const HexDirs& dirs);

    // static std::map<Color, std::map<CastlingEnum, Castling>> _castlings;
};

}  // namespace hexchess::core

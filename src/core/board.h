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

#include <cassert>

#include <array>
#include <functional>
#include <iomanip>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "fen.h"
#include "geometry.h"
#include "move.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"
#include "zobrist.h"


namespace hexchess::core {

/// \brief Stores the piece locations of a board using bitsets with one bit for each cell.
///
/// Multiple bitsets are used, including one that stores whether any piece is at a given index
/// (i.e., cell location), and one for each color that records whether any piece of that color
/// is present. Such bitsets are used together with some cached data structures that record
/// variant-specific board details, such as all the pseudo-legal moves available for a knight
/// from each of the positions on the board.
///
/// \todo Extend class to be able to support flat-side variants (e.g., Brusky chess and De Vasa chess).
/// \todo When adding support for Castling, consider adding a V::Bits field to Board called unmovedKingsAndRooks.
template <typename Variant>
class Board {
public:
    /// \brief typedef to make the Variant concisely available within the class.
    typedef Variant V;

    Board(bool doPopulate=true);
    Board(const Fen<V>& fen);
    Board(const std::string& fenStr);
    Board(const Board& other);
    ~Board() {}
    Board& operator=(const Board& other) = delete;

    // ========================================
    // Fundamental operations

    void initialize(const Fen<V>& fen);

    const std::string board_string() /* const */;

    void clear();

    HalfMoveCounter currentCounter() const;

    Color mover() const { return _mover; }

    const std::string pgn_string() const;

    // Record current Board state in Forsyth-Edwards Notation (FEN)
    const std::string fen_board_string() const;

    // Record current Board state in Forsyth-Edwards Notation (FEN)
    const std::string fen_string() const;

    // Record current Board state in Forsyth-Edwards Notation (FEN)
    const std::string board_bits_string() /* const */;

    bool pieceCount();
    bool pieceCount(Color c);

    // Note: Adding one more level to the decision tree (e.g., testing non-Pawns for isBN) doesn't pay off.
    Color getColorAt(Index index) const;
    PieceType getPieceTypeAt(Index index, Color c) const;
    PieceType getPieceTypeAt(Index index) const;

    // Note: Sets the appropriate positions in this class's boards to true or false, depending on \p value.
    void setPiece(Index index, Color c, PieceType pt, bool value);

    /// \brief Sets the appropriate bits in this class's boards to reflect that the given piece has been added.
    void addPiece(Index index, Color c, PieceType pt, bool verbose=false);

    /// \brief Resets bits in this class's boards to reflect that the given piece has been removed.
    void removePiece(Index index, Color c, PieceType pt);

    void reset(bool doPopulate) {
        clear();
        if (doPopulate) {
            Fen<V> fenInitial{Glinski::fenInitial};
            initialize(fenInitial);
        }
    }

    /// \todo Change return type to Fen<V>
    const Fen<V> fen() const;

    // ========================================
    // Piece locations

    /// \brief Returns a Board reflecting which board locations have any piece present.
    const typename V::Bits anyPieceBits()              const { return _anyPieceBits;              }

    /// \brief Returns a Board reflecting which board locations have any piece of the specified color.
    const typename V::Bits anyPieceBits(const Color c) const { return _colorToAnyPieceBits.at(c); }

    /// \brief Returns a Board reflecting which board location(s) have a King of the specified color.
    const typename V::Bits kingBits(const Color c)     const { return _colorToKingBits.at(c);     }

    /// \brief Returns a Board reflecting which board location(s) have a Queen of the specified color.
    const typename V::Bits queenBits(const Color c)    const { return _colorToQueenBits.at(c);     }

    /// \brief Returns a Board reflecting which board location(s) have a Rook of the specified color.
    const typename V::Bits rookBits(const Color c)     const { return _colorToRookBits.at(c);      }

    /// \brief Returns a Board reflecting which board location(s) have a Bishop of the specified color.
    const typename V::Bits bishopBits(const Color c)   const { return _colorToBishopBits.at(c);    }

    /// \brief Returns a Board reflecting which board location(s) have a Knight of the specified color.
    const typename V::Bits knightBits(const Color c)   const { return _colorToKnightBits.at(c);    }

    /// \brief Returns a Board reflecting which board location(s) have a Pawn of the specified color.
    const typename V::Bits pawnBits(const Color c)     const { return _colorToPawnBits.at(c);      }

    /// \brief Returns a Board reflecting which board location has an en passant cell.
    ///
    /// (Definition: An en passant cell is one that was skipped over by a Pawn in the opponent's
    /// previous move, in a variant that supports en passant capture.)
    const typename V::Bits enPassantBits() const { return _enPassantBits; }

    /// \brief Returns a boolean reflecting which board location(s) have a Pawn of the specified color.
    bool isAnyPieceAt(const Index index)               const { return _anyPieceBits.test(index);   }

    /// \brief Returns a boolean reflecting whether a piece with Color \p c is present at Index \p index.
    bool isColorAtIndex(const Index index, const Color c)  const { return _colorToAnyPieceBits.at(c).test(index); }

    /// \brief Returns a boolean reflecting whether a King with Color \p c is present at Index \p index.
    bool isKingAtIndex(const Index index, const Color c)   const { return _colorToKingBits.at(c).test(index);      }

    /// \brief Returns a boolean reflecting whether a Queen with Color \p c is present at Index \p index.
    bool isKQueenAtIndex(const Index index, const Color c) const { return _colorToQueenBits.at(c).test(index);     }

    /// \brief Returns a boolean reflecting whether a Rook with Color \p c is present at Index \p index.
    bool isRookAtIndex(const Index index, const Color c)   const { return _colorToRookBits.at(c).test(index);      }

    /// \brief Returns a boolean reflecting whether a Bishop with Color \p c is present at Index \p index.
    bool isBishopAtIndex(const Index index, const Color c) const { return _colorToBishopBits.at(c).test(index);    }

    /// \brief Returns a boolean reflecting whether a Knight with Color \p c is present at Index \p index.
    bool isKnightAtIndex(const Index index, const Color c) const { return _colorToKnightBits.at(c).test(index);    }

    /// \brief Returns a boolean reflecting whether a Pawn with Color \p c is present at Index \p index.
    bool isPawnAtIndex(const Index index, const Color c)   const { return _colorToPawnBits.at(c).test(index);      }

    /// \brief Returns the number of pieces on the board.
    Short anyPieceCount()        const { return _anyPieceBits.count();           }

    /// \brief Returns the number of pieces on the board with Color \p c.
    Short anyPieceCount(Color c) const { return _colorToAnyPieceBits[c].count(); }

    /// \brief Returns the number of Kings on the board with Color \p c.
    Short kingCount(Color c)     const { return _colorToKingBits[c].count();     }

    /// \brief Returns the number of Queens on the board with Color \p c.
    Short queenCount(Color c)    const { return _colorToQueenBits[c].count();    }

    /// \brief Returns the number of Rooks on the board with Color \p c.
    Short rookCount(Color c)     const { return _colorToRookBits[c].count();     }

    /// \brief Returns the number of Bishops on the board with Color \p c.
    Short bishopCount(Color c)   const { return _colorToBishopBits[c].count();   }

    /// \brief Returns the number of Knights on the board with Color \p c.
    Short knightCount(Color c)   const { return _colorToKnightBits[c].count();   }

    /// \brief Returns the number of Pawns on the board with Color \p c.
    Short pawnCount(Color c)     const { return _colorToPawnBits[c].count();     }

    bool isCastlingAvailable(Color c) {
        return _colorToRookCastlingAvailabilityBits.at(c).any();
    }

    /// \brief Returns true if the board is empty; otherwise, false.
    bool isEmpty() const { return anyPieceBits().none(); }

    /// \brief Returns true if location \p index is empty; otherwise, false.
    bool isEmpty(Index index) const { return !anyPieceBits()[index]; }

    /// \brief Returns whether mover is in check (inclusive of checkmate).
    bool isCheck() const;

    bool isInsufficientResources() const;

    Index getKingIndex(Color c) const { return _colorToKingIndex.at(c); }

    void setKingIndex(Color c, Index index) { _colorToKingIndex[c] = index; }

    /// \brief Returns the Zobrist hash of the board, given its current layout.
    ZHash zobristHash() const;

    // ========================================
    // Piece movement data stored as Bits and Indices

    /// \brief Returns a lookup for Player \p c: Which cells allow a Pawn to advance one cell forward.
    typename V::Bits pawnAdvance1Indices(Color c) {
        return V::_colorToPawnAdvance1Indices[c];
    }

    /// \brief Returns a lookup for Player \p c: Which cells allow a Pawn to advance two cells forward.
    typename V::Bits pawnAdvance2Indices(Color c) {
        return V::_colorToPawnAdvance2Indices[c];
    }

    /// \brief Returns a lookup for Player \p c: Which cells allow a Pawn to advance two cells forward.
    typename V::Bits pawnCaptureBits(Color c) {
        return V::_colorToPawnCaptureBits[c];
    }

    // ========================================
    // Compute move information

    // bool causesCheck(const Move& move);

    bool isCheckExclusive() const {
        return isCheckInclusive() && !isCheckmate();
    }

    bool isCheckmate() const;

    bool isCheckInclusive() const;

    bool isDrawByStalemate() const;

    bool isStalemate() const;

    /// \todo Implement Board::isPinned to support pinning as a board valuation feature
    bool isPinned(Index tgtInd, Color c) const;

    /// \todo Implement pinningIndices to support pinning as a board valuation feature
    Indices pinnningIndices(Index tgtInd, Color c) const;

    bool isAttacking(Index from, Color, PieceType pt, Index tgt) const;

    const Indices& attackers(Index index) const;

    /// \brief Outputs to \p moves_first (a collection of Move objects) the pseudo-legal moves for a
    ///     "leaper" piece at location \p index with PieceType \pt and Color \c.
    ///
    /// (The possible destinations of the piece are passed in as the argument \p dests.)
    template<class OutputMoveIt>
    void getLeapMoves(OutputMoveIt moves_first,
        Index from, Color mover, PieceType pt, Indices dests
        ) const;

    /// \brief Outputs to \p moves_first (a collection of Move objects) the pseudo-legal moves for a
    ///     "slider" piece at location \p index with PieceType \pt and Color \c.
    ///
    /// (The possible slide directions of the piece are passed in as the argument \p rays.)
    template<class OutputMoveIt>
    void getSlideMoves(OutputMoveIt moves_first,
        Index from, Color mover, PieceType pt, const HexRays<V>& rays
        ) const;

    /// \brief Outputs to \p moves_first (a collection of Move objects) the pseudo-legal moves for a
    ///     Pawn at location \p index with Color \c.
    ///
    /// (The Variant class has info on the Pawn's available advance and capture capabilities.)
    template<class OutputMoveIt>
    void getStandardPawnMoves(OutputMoveIt moves_first,
        Index from, Color mover
        ) const;

    /// \brief Outputs to \p moves_first (a collection of Move objects) the pseudo-legal moves for a
    ///     piece with PieceType \pt and Color \c at location \index.
    template<class OutputMoveIt>
    void getPseudoLegalMoves(OutputMoveIt moves_first,
        Index index, Color c, PieceType pt
        ) const;

    /// \todo PERFORMANCE: Check if this should direct output to an iterator argument
    template<class OutputMoveIt>
    void getPseudoLegalMoves(OutputMoveIt moves_first, Color c) const;

    Moves getPseudoLegalMoves(Color c) const;

    template<class OutputMoveIt>
    void getLegalMoves(OutputMoveIt moves_first, Color c) const;

    Moves getLegalMoves(Color c, const Moves& pseudoLegalMoves) const;

    Moves getLegalMoves(Color c) const;

    void moveMake(const Move& move);
    void moveRedo(const Move& move);
    void moveUndo(const Move& move);

    bool isRepetition() {}

    bool isDraw() { }
    bool isDrawBy3xRepetition() { }
    bool isDrawBy50NonProgressMoves() { }

    bool isCastlingAvailable() {
        return false;
        // TODO: Implement castling: return rookCastlingAvailabilityBits.any();
    }

    PiecesDense piecesDense() const;
    PiecesSparse piecesSparse() const;

private:
    // Setup
    // Fen<V> _fenInitial;

    // ========== Piece placement
    typename V::Bits _anyPieceBits;
    std::map<Color, typename V::Bits> _colorToAnyPieceBits;
    std::map<Color, typename V::Bits> _colorToKingBits;
    std::map<Color, typename V::Bits> _colorToQueenBits;
    std::map<Color, typename V::Bits> _colorToRookBits;
    std::map<Color, typename V::Bits> _colorToBishopBits;
    std::map<Color, typename V::Bits> _colorToKnightBits;
    std::map<Color, typename V::Bits> _colorToPawnBits;

    std::map<Color, Index> _colorToKingIndex;

    // ========== Game state
    Color _mover = Color::Black;
    // When a Rook is moved from index, and this Bits tests true at index, then that bit should be cleared.
    // When a King is moved, this Bits value should be cleared.
    std::map<Color, typename V::Bits> _colorToRookCastlingAvailabilityBits;

    // \todo Choose e.p. representation(s)
    OptIndex _optEpIndex{std::nullopt};
    typename V::Bits _enPassantBits{0};

    // ========== Game history
    /// \brief History of the game's moves

    /// \brief For each HalfMoveCounter, counter ticks since capture or Pawn move.
    Shorts _nonProgressCounters{};
    HalfMoveCounter _currentCounter{1};

    Moves _moveStack{};
    std::map<ZHash, std::vector<HalfMoveCounter>> _zHashToCounters{};  // To track repeats
    std::vector<ZHash> _zHashes{};  // One per counter, to track when a repeated Board position occurred

    // TODO: Useful cached computed info?
    CheckStatus _checkStatus{CheckStatus::Unknown};
    Moves _legalMoves{};
    Moves _pseudoLegalMoves{};
};

} // namespace hexchess::core

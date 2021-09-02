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
#include <utility>
#include <vector>

#include "fen.h"
#include "game_outcome.h"
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

    // ========================================
    // Constructor support

    // Note: Sets the appropriate positions in this class's boards to true or false, depending on \p value.
    void setPiece(Index index, Color c, PieceType pt, bool value);

    /// \brief Sets the appropriate bits in this class's boards to reflect that the given piece has been added.
    void addPiece(Index index, Color c, PieceType pt, bool verbose=false);

    /// \brief Resets bits in this class's boards to reflect that the given piece has been removed.
    void removePiece(Index index, Color c, PieceType pt);
    void initialize(const Fen<V>& fen);

    // ========================================
    // Constructors

    Board(bool doPopulate=true);
    Board(const Fen<V>& fen);
    Board(const std::string& fenStr);
    Board(const Board& other);
    ~Board() {}
    Board& operator=(const Board& other) = default;  // TODO: Implement

    // ========================================
    // Fundamental operations

    void clear();
    void reset(bool doPopulate);

    // ========================================
    // Non-piece data

    HalfMoveCounter currentCounter() const;
    Color mover() const { return _mover; }

    // ========================================
    // Write piece data

    void setKingIndex(Color c, Index index) { _colorToKingIndex[c] = index; }

    // ========================================
    // Piece movement capatibilities (stored as Bits and Indices)

    /// \brief Returns a lookup for Player \p c: Which cells allow a Pawn to advance one cell forward.
    Indices pawnAdvance1Indices(Index from, Color c) const {
        return V::_colorToPawnAdvance1Indices.at(c).at(from);
    }

    /// \brief Returns a lookup for Player \p c: Which cells allow a Pawn to advance two cells forward.
    Indices pawnAdvance2Indices(Index from, Color c) const {
        return V::_colorToPawnAdvance2Indices.at(c).at(from);
    }

    /// \brief Returns a lookup for Player \p c: Which cells allow a Pawn to advance two cells forward.
    Indices pawnCaptureBits(Index from, Color c) const {
        return V::_colorToPawnCaptureBits.at(c).at(from);
    }

    // ========================================
    // Read piece data

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

    // ========================================
    // Piece index queries

    /// \brief Returns a boolean reflecting which board location(s) have a Pawn of the specified color.
    bool isPieceAt(const Index index)                 const { return _anyPieceBits.test(index);   }

    /// \brief Returns a boolean reflecting whether a piece with Color \p c is present at Index \p index.
    bool isPieceAt(const Index index, const Color c)  const { return _colorToAnyPieceBits.at(c).test(index); }

    /// \brief Returns a boolean reflecting whether a King with Color \p c is present at Index \p index.
    bool isKingAt(const Index index, const Color c)   const { return _colorToKingBits.at(c).test(index);      }

    /// \brief Returns a boolean reflecting whether a Queen with Color \p c is present at Index \p index.
    bool isQueenAt(const Index index, const Color c) const { return _colorToQueenBits.at(c).test(index);     }

    /// \brief Returns a boolean reflecting whether a Rook with Color \p c is present at Index \p index.
    bool isRookAt(const Index index, const Color c)   const { return _colorToRookBits.at(c).test(index);      }

    /// \brief Returns a boolean reflecting whether a Bishop with Color \p c is present at Index \p index.
    bool isBishopAt(const Index index, const Color c) const { return _colorToBishopBits.at(c).test(index);    }

    /// \brief Returns a boolean reflecting whether a Knight with Color \p c is present at Index \p index.
    bool isKnightAt(const Index index, const Color c) const { return _colorToKnightBits.at(c).test(index);    }

    /// \brief Returns a boolean reflecting whether a Pawn with Color \p c is present at Index \p index.
    bool isPawnAt(const Index index, const Color c)   const { return _colorToPawnBits.at(c).test(index);      }

    Index getKingIndex(Color c) const { return _colorToKingIndex.at(c); }

    // ========================================
    // Piece counts

    /// \brief Returns the number of pieces on the board.
    Short pieceCount()           const { return _anyPieceBits.count(); }

    /// \brief Returns the number of pieces on the board with Color \p c.
    Short pieceCount(Color c)    const { return _colorToAnyPieceBits.at(c).count(); }

    /// \brief Returns the number of Kings on the board with Color \p c.
    Short kingCount(Color c)     const { return _colorToKingBits.at(c).count();     }

    /// \brief Returns the number of Queens on the board with Color \p c.
    Short queenCount(Color c)    const { return _colorToQueenBits.at(c).count();    }

    /// \brief Returns the number of Rooks on the board with Color \p c.
    Short rookCount(Color c)     const { return _colorToRookBits.at(c).count();     }

    /// \brief Returns the number of Bishops on the board with Color \p c.
    Short bishopCount(Color c)   const { return _colorToBishopBits.at(c).count();   }

    /// \brief Returns the number of Knights on the board with Color \p c.
    Short knightCount(Color c)   const { return _colorToKnightBits.at(c).count();   }

    /// \brief Returns the number of Pawns on the board with Color \p c.
    Short pawnCount(Color c)     const { return _colorToPawnBits.at(c).count();     }

    // ========================================
    // Other piece location methods

    Color getColorAt(Index index) const;
    PieceType getPieceTypeAt(Index index, Color c) const;
    PieceType getPieceTypeAt(Index index) const;

    PiecesDense piecesDense() const;
    PiecesDense piecesDense(Color c) const;
    PiecesSparse piecesSparse() const;

    /// \todo Change return type to Fen<V>
    const Fen<V> fen() const;

    /// \brief Returns true if the board is empty; otherwise, false.
    bool isEmpty() const { return anyPieceBits().none(); }

    /// \brief Returns true if location \p index is empty; otherwise, false.
    bool isEmpty(Index index) const { return !anyPieceBits()[index]; }

    // ========================================
    // Board hashing

    /// \brief Returns the Zobrist hash of the board, given its current layout.
    ZHash zobristHash() const;

    bool isRepetition() const;

    // ========================================
    // String methods

    // Record current Board state in Forsyth-Edwards Notation (FEN)
    const std::string board_bits_string() /* const */;

    const std::string board_pgn_string() const;

    const std::string board_string() /* const */;

    // Record current Board state in Forsyth-Edwards Notation (FEN)
    const std::string fen_string() const;

    // ========================================
    // Finding, getting, and caching moves

    void recordObstructedHexRayCore(Index obsIndex, Index rayStart, const HexDir& rayDir) const;
    const HexRayCores& getObstructedHexRayCores(Index obsIndex) const;

    /// \brief Outputs to \p moves_first (a collection of Move objects) the pseudo-legal moves for a
    ///     "leaper" piece at location \p index with PieceType \pt and Color \c.
    ///
    /// (The possible destinations of the piece are passed in as the argument \p dests.)
    void findLeapMoves(/* out */ Moves& moves,
        Index from, Color mover, PieceType pt, const Indices& dests
        ) const;

    /// \brief Outputs to \p moves_first (a collection of Move objects) the pseudo-legal moves for a
    ///     "slider" piece at location \p index with PieceType \pt and Color \c.
    ///
    /// (The possible slide directions of the piece are passed in as the argument \p rays.)
    void findSlideMoves(/* out */ Moves& moves,
        Index from, Color mover, PieceType pt, const HexRays<V>& rays
        ) const;

    /// \brief Outputs to \p moves_first (a collection of Move objects) the pseudo-legal moves for a
    ///     Pawn at location \p index with Color \c.
    ///
    /// (The Variant class has info on the Pawn's available advance and capture capabilities.)
    void findStandardPawnMoves(/* out */ Moves& moves,
        Index from, Color mover
        ) const;

    /// \brief Outputs to \p moves_first (a collection of Move objects) the pseudo-legal moves for a
    ///     piece with PieceType \pt and Color \c at location \index.
    void findPseudoLegalMoves(/* out */ Moves& moves,
        Index index, Color c, PieceType pt
        ) const;

    /// \todo PERFORMANCE: Check if this should direct output to an iterator argument
    void findPseudoLegalMoves(/* out */ Moves& moves, Color c) const;

    void recordPseudoLegalMoves(const Moves& moves) const;
    Moves getPseudoLegalMoves(Color mover) const;  // Get through cache

    // ----------------------------------------

    bool isOwnKingAttackedAfterOwnMove(Color mover, Index from, Index to) const;
    void findLegalMoves(/* out */ Moves& moves, Color c, const Moves& pseudoLegalMoves) const;
    void recordLegalMoves(const Moves& moves) const;
    Moves getLegalMoves(Color mover) const;  // Get through cache

    bool isAttacked(Index tgtIndex, Color tgtColor) const;
    bool isAttacking(Index from, Color, PieceType pt, Index tgt) const;

    // ----------------------------------------

    CheckEnum getCheckEnum() const;
    void recordCheckEnum(CheckEnum checkEnum) const;

    // ----------------------------------------

    OptGameOutcome getOptOutcome() const { return _cache.optOutcome; }
    void recordOutcome(const GameOutcome& gameOutcome) const;
    GameOutcome getOutcome() const { return _cache.optOutcome.value(); }

    // ----------------------------------------

    bool getIsGameOver() const;

    // ========================================
    // Move execution

    void moveExec(Move& move);
    void moveRedo(const Move& move);
    void moveUndo(const Move& move);

    // ========================================
    // Reading and writing game state

    const Indices& attackers(Index tgtInd, const Moves& moves) const;

    // bool causesCheck(const Move& move);

    // TODO: Add support for castling
    // When a Rook is moved from index, and this Bits tests true at index,
    //     then that bit should be cleared.
    // When a King is moved, this Bits value should be cleared.
    // std::map<Color, typename V::Bits> _colorToRookCastlingAvailabilityBits;
    // bool isCastlingAvailable(Color c) {
    //     return _colorToRookCastlingAvailabilityBits.at(c).any();
    // }
    bool isCastlingAvailable() {
        return false;
        // TODO: Implement castling: return rookCastlingAvailabilityBits.any();
    }

    /// \brief Returns whether mover is in check (inclusive of checkmate).
    bool isCheck() const;

    bool isCheckmate() const;

    bool isDraw() const {
        if (_cache.getOutcome() == std::nullopt) {
            return false;
        }
        Termination term = getOptOutcome().value().termination();
        return term == Termination::Draw_3xBoardRepetition
                    || term == Termination::Draw_50MoveRule
            || term == Termination::Draw_InsufficientResources || term == Termination::Draw_Stalemate;
    }

    bool isDrawBy3xBoardRepetition() {
        if (_cache.getOutcome() == std::nullopt) { return false; }
        return getOptOutcome().value().termination == Termination::Draw_3xBoardRepetition;
    }

    bool isDrawBy50NonProgressMoves() const {
        if (_cache.getOutcome() == std::nullopt) { return false; }
        return getOptOutcome().value().termination == Termination::Draw_50MoveRule;
    }

    bool isDrawByInsufficientResources() const;

    bool isDrawByStalemate() const;

    bool isGameOver() const { return getOptOutcome() != std::nullopt; }

    /// \todo Implement Board::isPinned to support pinning as a board valuation feature
    // bool isPinned(Index tgtInd, Color c) const;

    /// \todo Implement pinningIndices to support pinning as a board valuation feature
    // Indices pinnningIndices(Index tgtInd, Color c) const;

    void setMoveCheckEnum(Move& move);

private:
    // =======================================
    // Piece locations

    typename V::Bits _anyPieceBits;
    std::map<Color, typename V::Bits> _colorToAnyPieceBits;
    std::map<Color, typename V::Bits> _colorToKingBits;
    std::map<Color, typename V::Bits> _colorToQueenBits;
    std::map<Color, typename V::Bits> _colorToRookBits;
    std::map<Color, typename V::Bits> _colorToBishopBits;
    std::map<Color, typename V::Bits> _colorToKnightBits;
    std::map<Color, typename V::Bits> _colorToPawnBits;

    std::map<Color, Index> _colorToKingIndex;

    // =======================================
    // Move piece support

    void _moveBit(std::map<Color, typename Glinski::Bits>& bits,
        Color mover, Index from, Index to);

    void _moveBit(typename Glinski::Bits& bits,
        Index from, Index to);

    // =======================================
    // Non-piece data

    Color _mover /* = Color::Black */;

    /// \brief Returns which board location (if any) has an en passant cell.
    ///
    /// (Definition: An en passant cell is one that was skipped over by a Pawn in the opponent's
    /// previous move, in a variant that supports en passant capture.)
    /// Put en passant code here
    OptIndex _optEpIndex{std::nullopt};

    // =======================================
    // Game history

    Moves _moveStack{};

    /// \brief For each HalfMoveCounter, counter ticks since capture or Pawn move.
    Short _nonProgressCounter{0};
    Shorts _nonProgressCounters{};
    HalfMoveCounter _currentCounter{1};

    // Hash history
    std::map<ZHash, std::vector<HalfMoveCounter>> _zHashToCounters{};  // To track repeats
    std::vector<ZHash> _zHashes{};  // One per counter, to track when a repeated Board position occurred

    // =======================================
    // Caching

    /// Each cache item is initliazed to empty, and filled via getter calls.
    /// This caching layer allows (computer) Players to determine if and when
    ///     they compute this info for games inspected during game tree search.
    struct Cache {
        Cache()
            : obstructedHexRayMap{},
              optPseudoLegalMoves{},
              optLegalMoves{},
              optCheckEnum{},
              optOutcome{}
        { }

        void clear() {
            obstructedHexRayMap.clear();
            optPseudoLegalMoves = std::nullopt;
            optLegalMoves = std::nullopt;
            optCheckEnum = std::nullopt;
            optOutcome = std::nullopt;
        }

        /// \brief Storage of slides of pieces that are blocked by opponent's pieces.
        ///
        /// This pseudo-legal moves that are *not* legal are those that unblock
        /// attacks by the opponent's slider pieces. So we cache such incidents
        /// in the form (objstructionIndex, (startIndex, dir)).
        ///
        /// Unlike the other values in the Board Cache, this one is not wrapped
        /// by std::optional, because its computation is not computed in a single
        /// function call, but rather distributed across the construction of the
        /// HexRays of the sliding pieces.
        ObstructedHexRayMap obstructedHexRayMap{};

        OptMoves       optPseudoLegalMoves{};
        OptMoves       optLegalMoves{};
        OptCheckEnum   optCheckEnum{};
        OptGameOutcome optOutcome{};
    };
    mutable Cache _cache{};
};

} // namespace hexchess::core

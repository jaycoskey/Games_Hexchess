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
#include <stdexcept>
#include <string>
#include <vector>

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
/// \todo Have all forms of Bitboard::init() either require the Bitboard is empty, or clear its contents.
/// \todo Add a Bitboard constructor that takes a FEN string.
/// \todo Add a Bitboard constructor that takes arguments that reflect the six components of a FEN string.
/// \todo Extend class to be able to support flat-side variants (e.g., Brusky chess and De Vasa chess).
/// \todo When adding support for Castling, consider adding a V::Bits field to Bitboard called unmovedKingsAndRooks.
/// \todo Allow determining initial piece placement by FEN string
template <typename Variant>
class Bitboard {
public:
    /// \brief typedef to make the Variant concisely available within the class.
    typedef Variant V;

    Bitboard(bool doPopulate=true);
    Bitboard(const Bitboard& other);
    ~Bitboard() {}
    Bitboard& operator=(const Bitboard& other) = delete;

    // ========================================
    // Fundamental operations

    bool init(std::vector<std::pair<Color, PieceType>>, HalfMoveCounter nonProgressCounter=0);

    bool init(const std::string& fen);

    const std::string bitboard_string() /* const */;

    // Record current Board state in Forsyth-Edwards Notation (FEN)
    const std::string fen_string() /* const */;

    // Record current Board state in Forsyth-Edwards Notation (FEN)
    const std::string piecebits_string() /* const */;

    bool pieceCount();
    bool pieceCount(Color c);

    // Note: Adding one more level to the decision tree (e.g., testing non-Pawns for isBN) doesn't pay off.
    Color getColorAt(Index index) const;
    PieceType getPieceTypeAt(Index index, Color c) const;
    PieceType getPieceTypeAt(Index index) const;

    // Note: Sets the appropriate positions in this class's bitboards to true or false, depending on \p value.
    void setPiece(Index index, Color c, PieceType pt, bool value);

    /// \brief Sets the appropriate bits in this class's bitboards to reflect that the given piece has been added.
    void addPiece(Index index, Color c, PieceType pt);

    /// \brief Zeroes the appropriate bits in this class's bitboards to reflect that the given piece has been removed.
    void removePiece(Index index, Color c, PieceType pt);

    // ========================================

    /// \brief Returns a Bitboard reflecting which board locations have any piece present.
    const typename V::Bits anyPieceBits()              const { return _anyPieceBits;           }

    /// \brief Returns a Bitboard reflecting which board locations have any piece of the specified color.
    const typename V::Bits anyPieceBits(const Color c) const { return _colorToAnyPieceBits.at(c); }

    /// \brief Returns a Bitboard reflecting which board location(s) have a King of the specified color.
    const typename V::Bits kingBits(const Color c)     const { return _colorToKingBits.at(c);     }

    /// \brief Returns a Bitboard reflecting which board location(s) have a Queen of the specified color.
    const typename V::Bits queenBits(const Color c)    const { return _colorToQueenBits.at(c);    }

    /// \brief Returns a Bitboard reflecting which board location(s) have a Rook of the specified color.
    const typename V::Bits rookBits(const Color c)     const { return _colorToRookBits.at(c);     }

    /// \brief Returns a Bitboard reflecting which board location(s) have a Bishop of the specified color.
    const typename V::Bits bishopBits(const Color c)   const { return _colorToBishopBits.at(c);   }

    /// \brief Returns a Bitboard reflecting which board location(s) have a Knight of the specified color.
    const typename V::Bits knightBits(const Color c)   const { return _colorToKnightBits.at(c);   }

    /// \brief Returns a Bitboard reflecting which board location(s) have a Pawn of the specified color.
    const typename V::Bits pawnBits(const Color c)     const { return _colorToPawnBits.at(c);     }

    /// \brief Returns a Bitboard reflecting which board location has an en passant cell.
    ///
    /// (Definition: An en passant cell is one that was skipped over by a Pawn in the opponent's
    /// previous move, in a variant that supports en passant capture.)
    const typename V::Bits enPassantBits() const { return _enPassantBits; }

    /// \brief Returns a boolean reflecting which board location(s) have a Pawn of the specified color.
    bool isAnyPieceAtIndex(const Index index)              const { return _anyPieceBits.test(index);              }

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

    /// \brief Returns true if the board is empty; otherwise, false.
    bool isEmpty() const { return anyPieceBits() == V::zeroBits; }

    /// \brief Returns true if location \p index is empty; otherwise, false.
    bool isEmpty(Index index) const { return !anyPieceBits()[index]; }

    /// \brief Returns the Zobrist hash of the board, given its current layout.
    ZHash zobristHash() const;

    // ========================================
    // Piece movement data stored as Bits and Indices

    /// \brief Returns a lookup for Player \c: Which cells allow a Pawn to advance one cell forward.
    typename V::Bits pawnAdvance1Indices(Color c) {
        return _colorToPawnAdvance1Indices[c];
    }

    /// \brief Returns a lookup for Player \c: Which cells allow a Pawn to advance two cells forward.
    typename V::Bits pawnAdvance2BIndices(Color c) {
        return _colorToPawnAdvance2Indices[c];
    }

    /// \brief Returns a lookup for Player \c: Which cells allow a Pawn to advance two cells forward.
    typename V::Bits pawnCaptureBits(Color c) {
        return _colorToPawnCaptureBits[c];
    }

    // ========================================
    // Get moves

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

    /// \brief Outputs to \p moves_first (a collection of Move objects) the pseudo-legal moves for
    ///     all pieces of the specified Color \c.
    template<class OutputMoveIt>
    void getPseudoLegalMoves(OutputMoveIt moves_first, Color c) const;

    /// \brief Outputs to \p moves_first (a collection of Move objects) the legal moves
    ///     for all pieces of the specified Color \p c.
    template<class OutputMoveIt>
    void getLegalMoves(OutputMoveIt moves_first, Color c) const;

private:
    Color mover;

    typename V::Bits _anyPieceBits;
    std::map<Color, typename V::Bits> _colorToAnyPieceBits;
    std::map<Color, typename V::Bits> _colorToKingBits;
    std::map<Color, typename V::Bits> _colorToQueenBits;
    std::map<Color, typename V::Bits> _colorToRookBits;
    std::map<Color, typename V::Bits> _colorToBishopBits;
    std::map<Color, typename V::Bits> _colorToKnightBits;
    std::map<Color, typename V::Bits> _colorToPawnBits;

    std::map<Color, std::map<Index, Index>> _colorToPawnAdvance1Indices;
    std::map<Color, std::map<Index, Index>> _colorToPawnAdvance2Indices;
    std::map<Color, typename V::Bits> _colorToPawnCaptureBits;

    typename V::Bits _enPassantBits;
};

} // namespace hexchess::core

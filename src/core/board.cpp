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

#include <cctype>

#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include "board.h"
#include "fen.h"
#include "geometry.h"
#include "move.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"
#include "zobrist.h"

using std::cout;
using std::ostringstream;
using std::string;

using std::set;
using std::vector;


namespace hexchess::core {

// ========================================
// Constructor support

// Note: Does not check prior status of Board
template<>
void Board<Glinski>::setPiece(Index index, Color c, PieceType pt, bool value) {
    _anyPieceBits[index] = value;
    _colorToAnyPieceBits[c][index] = value;

    switch(pt) {
    case PieceType::Pawn:
        _colorToPawnBits[c][index] = value;
        break;
    case PieceType::Knight:
        _colorToKnightBits[c][index] = value;
        break;
    case PieceType::Bishop:
        _colorToBishopBits[c][index] = value;
        break;
    case PieceType::Rook:
        _colorToRookBits[c][index] = value;
        break;
    case PieceType::Queen:
        _colorToQueenBits[c][index] = value;
        break;
    case PieceType::King:
        _colorToKingBits[c][index] = value;
        break;
    default:
        throw std::logic_error("Board<Glinski>::setPiece: Unrecognized piece type");
    }
}

template<>
void Board<Glinski>::addPiece(Index index, Color c, PieceType pt, bool verbose) {
    if (verbose) {
        cout << "Adding " << piece_string(c, pt)
             << " @ " << V::cellNames[index] << "\n";
    }
    setPiece(index, c, pt, true);
}

template<>
void Board<Glinski>::removePiece(Index index, Color c, PieceType pt) {
    setPiece(index, c, pt, false);
}

template<>
void Board<Glinski>::initialize(const Fen<Glinski>& fen) {
    // (1) Piece placement
    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        if (fen.piecesSparse[index].has_value()) {
            auto [c, pt] = fen.piecesSparse[index].value();
            addPiece(index, c, pt);
        }
    }

    // (2) Mover
    _mover = fen.mover;

    // (3) TODO: Castling availability

    // (4) En passant avaiability
    _optEpIndex = fen.optEpIndex;

    // (5) Half-move (ply) clock
    _currentCounter = fen.currentCounter;

    // (6) Full move number (not needed)
}

template<>
Color Board<Glinski>::getColorAt(Index index) const {
    assert(anyPieceBits().test(index));
    return anyPieceBits(Color::Black).test(index)
        ? Color::Black
        : Color::White;
}

// Adding one more level to the decision tree (e.g., testing non-Pawns for isBN) doesn't pay off.
template<>
PieceType Board<Glinski>::getPieceTypeAt(Index index, Color c) const {
    assert(anyPieceBits().test(index));
    assert(anyPieceBits(c).test(index));

    if (pawnBits(c).test(index)) {
        return PieceType::Pawn;
    } else if (knightBits(c).test(index)) {
        return PieceType::Knight;
    } else if (bishopBits(c).test(index)) {
        return PieceType::Bishop;
    } else if (rookBits(c).test(index)) {
        return PieceType::Rook;
    } else if (queenBits(c).test(index)) {
        return PieceType::Queen;
    } else /* King */ {
        if (!kingBits(c).test(index)) {
            throw std::logic_error("Inconsistent board information on cell #" + std::to_string(index));
        }
        return PieceType::King;
    }
}

template<>
PieceType Board<Glinski>::getPieceTypeAt(Index index) const {
    switch(getColorAt(index)) {
    case Color::Black:
        return getPieceTypeAt(index, Color::Black);
    case Color::White:
        return getPieceTypeAt(index, Color::White);
    default:
        throw std::logic_error("Board<Glinski>::getPieceTypeAt: Unrecognized Color");
    }
}

// ========================================
// Constructors

/// \brief Constructs a Board---populated if \p doPopulate is true
///
/// For a board with default initial setup: Board(bool doPopulate=true[default])
/// For an empty board:                     Board(bool doPopulate=false)
/// For a board with custom setup, use Board(const Fen<V> initial setup: Board()
template<>
Board<Glinski>::Board(bool doPopulate)
  : _anyPieceBits{},
    _colorToAnyPieceBits { { Color::Black, {}}, {Color::White, {}} },
    _colorToKingBits     { { Color::Black, {}}, {Color::White, {}} },
    _colorToQueenBits    { { Color::Black, {}}, {Color::White, {}} },
    _colorToRookBits     { { Color::Black, {}}, {Color::White, {}} },
    _colorToBishopBits   { { Color::Black, {}}, {Color::White, {}} },
    _colorToKnightBits   { { Color::Black, {}}, {Color::White, {}} },
    _colorToPawnBits     { { Color::Black, {}}, {Color::White, {}} },

    _colorToKingIndex    { { Color::Black, -1}, {Color::White, -1} },

    _mover{Color::White},
    _colorToRookCastlingAvailabilityBits
                         { { Color::Black, {}}, {Color::White, {}} },

    _optEpIndex{std::nullopt},
    _enPassantBits{0}
{
    if (doPopulate) {
        Fen<V> fenInitial{Glinski::fenInitial};
        initialize(fenInitial);
    }
}

/// \brief Constructs Board from Fen instance (derived from FEN string)
template<>
Board<Glinski>::Board(const Fen<V>& fen)
    : Board<Glinski>::Board(false)
{
    // Part (1) of FEN string: Piece placements
    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        if (fen.piecesSparse[index].has_value()) {
            const auto [c, pt] = fen.piecesSparse[index].value();
            addPiece(index, c, pt);

            assert(getColorAt(index) == c);
            assert(getPieceTypeAt(index) == pt);
            assert(getPieceTypeAt(index, c) == pt);

            if (pt == PieceType::King) {
                if (c == Color::Black) {
                    _colorToKingIndex[Color::Black] = index;
                } else {
                    _colorToKingIndex[Color::White] = index;
                }
            }
        }
    }

    // Part (2) of FEN string: Active color
    _mover = fen.mover;

    // Part (3) of FEN string: Castling availability: TODO
    // _colorToRookCastlingAvailabilityBits[Color::Black].set(k);
    // _colorToRookCastlingAvailabilityBits[Color::White].set(k);

    // Part (4) of FEN string: En passant
    _optEpIndex = std::nullopt;

    // Part (5) of FEN string: Half move clock
    _currentCounter = fen.currentCounter;

    // Part (6) of FEN string: Full move counter
    // Not needed
}

/// \brief Constructs Board from FEN string by delegating to constructor using Fen instance
template<>
Board<Glinski>::Board(const string& fenStr)
    : Board<Glinski>::Board{Fen<V>{fenStr}}
{ }

// ========================================

template<>
PiecesDense Board<Glinski>::piecesDense() const {
    PiecesDense result{};

    for (Index index = 0; index < V::CELL_COUNT; ++index) {
        if (isAnyPieceAt(index)) {
            Color c = getColorAt(index);
            PieceType pt = getPieceTypeAt(index, c);
            result.push_back(std::make_tuple(index, c, pt));
        }
    }
    return result;
}

template<>
bool Board<Glinski>::isAttacking(Index from, Color, PieceType pt, Index tgt) const {
    throw NotImplementedException{"Board<Glinski>::isAttacking"};
}

template<>
bool Board<Glinski>::isCheck() const {
    Index kIndex = getKingIndex(_mover);
    assert(getPieceTypeAt(kIndex) == PieceType::King);

    const PiecesDense& pieces = piecesDense();
    for (const auto& pieceInfo : pieces) {
        auto [from, c, pt] = pieceInfo;
        if (c == opponent(_mover)) {
            if (isAttacking(from, c, pt, kIndex)) {
                return true;
            }
        }
    }
    return false;
}

template<>
bool Board<Glinski>::isCheckInclusive() const {
    return isCheck();
}

template<>
bool Board<Glinski>::isCheckmate() const {
    throw NotImplementedException{"Board<Glinski>::isCheckmate"};
}

template<>
bool Board<Glinski>::isCheckExclusive() const {
    return isCheckInclusive() && !isCheckmate();
}

// ========================================

template<>
void Board<Glinski>::clear() {
    _anyPieceBits.reset();
    _enPassantBits.reset();

    for (Color c : {Color::Black, Color::White}) {
        _colorToAnyPieceBits[c].reset();
        _colorToKingBits[c].reset();
        _colorToQueenBits[c].reset();
        _colorToRookBits[c].reset();
        _colorToBishopBits[c].reset();
        _colorToKnightBits[c].reset();
        _colorToPawnBits[c].reset();
    }
    /// \todo: Support castling: Have Board::clear() modfy _castlingBits
}

template<>
HalfMoveCounter Board<Glinski>::currentCounter() const {
    return _currentCounter;
}

template<>
const string Board<Glinski>::pgn_string() const {
    std::ostringstream oss;

    oss << "[TODO: Print headers]\n";
    oss << "\n";
    for (Short k = 0; k < _moveStack.size(); ++k) {
        if (k % 2 == 0) {
            oss << _moveStack[k] << ". ";
        } else {
            oss << _moveStack[k] << " ";
        }
        oss << _moveStack[k].move_lan_string();
        if (k % 2 == 1) {
            oss << "\n";
        }
    }
    if (_moveStack.size() % 2 == 0) {
        oss << "\n";
    }
    return oss.str();
}

template<>
PiecesSparse Board<Glinski>::piecesSparse() const {
    PiecesSparse result{};

    for (Index index = 0; index < V::CELL_COUNT; ++index) {
        if (isAnyPieceAt(index)) {
            Color c = getColorAt(index);
            PieceType pt = getPieceTypeAt(index, c);
            result.push_back(mkPair(c, pt));
        } else {
            result.push_back(std::nullopt);
        }
    }
    return result;
}

template<>
const Fen<Glinski> Board<Glinski>::fen() const {
    return Fen<V>{piecesSparse(), _mover, _optEpIndex, _currentCounter};
}

// ========================================
// PieceType-based move generators

template<>
template<class OutputMoveIt>
void Board<Glinski>::getLeapMoves(
    OutputMoveIt moves_first,
    Index from, Color mover,
    PieceType pt, Indices dests
    ) const
{
    // TODO: Implement method
    throw NotImplementedException{"Board<Glinski>::getLeapMoves"};
}

template<>
template<class OutputMoveIt>
void Board<Glinski>::getStandardPawnMoves(
    OutputMoveIt moves_first,
    Index from,
    Color mover
    ) const
{
    // TODO: Implement method
    throw NotImplementedException{"Board<Glinski>::getStandardPawnMoves"};

    // for (Index to : pawnCaptures) {
    //     if (isEnemy(to)) {
    //         Move move{c, PieceType::Pawn, from, to, isCapture=true};
    //         result.append(move);
    //     }
    //     OptIndex enPassantIndex = getLastMove().enPassantInfo();
    //     if (enPassantCell.has_value() && enPassantCell.value == to) {
    //         Move move{c, PieceType::Pawn, from, to, isCapture=true, isEnPassant=true};
    //         result.append(move);
    //     }
    // }
    // for (Index to : pawnAdvances) {
    //     if isEmpty(to) {
    //         result.append(Move(c, PieceType::Pawn, from, to));
    //     }
    // }
}

template<>
template<class OutputMoveIt>
void Board<Glinski>::getSlideMoves(
    OutputMoveIt moves_first,
    Index from,
    Color mover,
    PieceType pt,
    const HexRays<Glinski>& rays
    ) const
{
    throw NotImplementedException{"Board<Glinski>::getSlideMoves"};
    // TODO: Implement method
    // for (HexRay ray : rays) {
    //     for (dest : ray) {
    //         if isEmpty(dest) {
    //             result.append(...);
    //         }
    //         if isEnemyAt(dest) {
    //             result.append(...);
    //             break;
    //         }
    //         break;  // Own piece is at dest
    //     }
    // }
}

// ========================================

template<>
const string Board<Glinski>::board_string() {
    ostringstream oss;
    int rowNum = 0;
    int cellsRemainingInRow = Glinski::fenRowLengths[rowNum];
    assert(cellsRemainingInRow == 1);

    auto indent = [](int r) {
        Size indent = 2;
        Short rowsFromCenter = abs(r - 10);
        if (rowsFromCenter > 5) {
            indent += 2 * (rowsFromCenter - 5);
        } else {
            indent += (r % 2 == 0) ? 2 : 0;
        }
        return indent;
    };

    oss << std::setw(2) << rowNum << ": "
        << "indent=" << std::setw(2) << indent(0) << ": "
        << std::setw(indent(0)) << " ";
    for (Index index : Glinski::fenOrderToIndex) {
        if (isEmpty(index)) {
            oss << "--  ";
        } else {
            Color c = getColorAt(index);
            PieceType pt = getPieceTypeAt(index, c);
            oss << piece_string(c, pt) << "  ";
        }
        cellsRemainingInRow--;

        if (cellsRemainingInRow == 0) {
            oss << "\n";
            rowNum++;
            if (rowNum < Glinski::ROW_COUNT) {
                oss << std::setw(2) << rowNum << ": "
                    << "indent=" << std::setw(2) << indent(rowNum) << ": "
                    << std::setw(indent(rowNum)) << " ";
            }
            cellsRemainingInRow = Glinski::fenRowLengths[rowNum];
        }
    }
    return oss.str();
}

template<>
const string Board<Glinski>::fen_string() const {
    return Fen<V>{fen()}.fen_string();
}

template<>
const string Board<Glinski>::board_bits_string() /* const */ {
    ostringstream oss;

    oss << "Any Piece : " << reved(_anyPieceBits.to_string())  << "\n";
    oss << "En Passant: " << reved(_enPassantBits.to_string()) << "\n";
    oss << "    Black:\n";
    oss << "\tA:  " << reved(_colorToAnyPieceBits[Color::Black].to_string()) << "\n";
    oss << "\tK:  " << reved(_colorToKingBits[Color::Black].to_string())     << "\n";
    oss << "\tQ:  " << reved(_colorToQueenBits[Color::Black].to_string())    << "\n";
    oss << "\tR:  " << reved(_colorToRookBits[Color::Black].to_string())     << "\n";
    oss << "\tB:  " << reved(_colorToBishopBits[Color::Black].to_string())   << "\n";
    oss << "\tN:  " << reved(_colorToKnightBits[Color::Black].to_string())   << "\n";
    oss << "\tP:  " << reved(_colorToPawnBits[Color::Black].to_string())     << "\n";
    oss << "    White:\n";
    oss << "\tA:  " << reved(_colorToAnyPieceBits[Color::White].to_string()) << "\n";
    oss << "\tK:  " << reved(_colorToKingBits[Color::White].to_string())     << "\n";
    oss << "\tQ:  " << reved(_colorToQueenBits[Color::White].to_string())    << "\n";
    oss << "\tR:  " << reved(_colorToRookBits[Color::White].to_string())     << "\n";
    oss << "\tB:  " << reved(_colorToBishopBits[Color::White].to_string())   << "\n";
    oss << "\tN:  " << reved(_colorToKnightBits[Color::White].to_string())   << "\n";
    oss << "\tP:  " << reved(_colorToPawnBits[Color::White].to_string())     << "\n";

    return oss.str();
}

// ========================================
// Move generation

template<>
template<class OutputMoveIt>
void Board<Glinski>::getPseudoLegalMoves(OutputMoveIt moves_first,
    Index from, Color mover, PieceType pt
    ) const
{
    switch (pt) {
    case PieceType::King:
        getLeapMoves(moves_first, from, mover, pt, Glinski::kingLeapDirs);
        break;
    case PieceType::Queen:
        getSlideMoves(moves_first, from, mover, pt, Glinski::queenRays);
        break;
    case PieceType::Rook:
        getSlideMoves(moves_first, from, mover, pt, Glinski::rookRays);
        break;
    case PieceType::Bishop:
        getSlideMoves(moves_first, from, mover, pt, Glinski::bishopRays);
        break;
    case PieceType::Knight:
        getLeapMoves(moves_first, from, mover, pt, Glinski::knightDests);
        break;
    case PieceType::Pawn:
        getStandardPawnMoves(moves_first, from, mover, pt);
        break;
    default:
        throw std::logic_error("Board: pseudoLegalDestinations: Unrecognized PieceType");
    }
}

template<>
Moves Board<Glinski>::getPseudoLegalMoves(Color c) const {
    // TODO: Implement method
    throw NotImplementedException{"Board<Glinski>::getPseudoLegalMoves"};
}

template<>
template<class OutputMoveIt>
void Board<Glinski>::getLegalMoves(OutputMoveIt move_first, Color c) const {
    // TODO: Implement method
    throw NotImplementedException{"Board<Glinski>::getLegalMoves(Iterator, Color)"};
}

template<>
Moves Board<Glinski>::getLegalMoves(Color c, const Moves& pseudoLegalMoves) const {
    // TODO: Implement method
    throw NotImplementedException{"Board<Glinski>::getLegalMoves(Color, const Move&)"};
}

template<>
Moves Board<Glinski>::getLegalMoves(Color c) const {
    // TODO: Implement method
    throw NotImplementedException{"Board<Glinski>::getLegalMoves(Color)"};
}

// ========================================

// template<>
// bool Board<Glinski>::causesCheck(const Move& move) {
//     throw NotImplementedException{"Board<Glinski>::causesCheck(Color)"};
// }

template<>
bool Board<Glinski>::isDrawByStalemate() const {
    throw NotImplementedException{"Board<Glinski>::isDrawByStalemate"};
}

template<>
bool Board<Glinski>::isStalemate() const {
    throw NotImplementedException{"Board<Glinski>::isStalemate"};
}

template<>
bool Board<Glinski>::isPinned(Index tgtInd, Color c) const {
    throw NotImplementedException{"Board<Glinski>::isPinned"};
}

template<>
Indices Board<Glinski>::pinnningIndices(Index tgtInd, Color c) const {
    throw NotImplementedException{"Board<Glinski>::pinningIndides"};
}

template<>
const Indices& Board<Glinski>::attackers(Index index) const {
    throw NotImplementedException{"Board<Glinski>::attackers"};
}

// ========================================


template<>
bool Board<Glinski>::isInsufficientResources() const {
    throw NotImplementedException{"Board<Glinski>::insufficientResources"};
}

// ========================================
// Zobrist hash generation

template<>
ZHash Board<Glinski>::zobristHash() const {
    ZHash result = 0;
    for (Index index = 0; index < V::CELL_COUNT; ++index) {
        if (!isEmpty(index)) {
            Color c = getColorAt(index);
            PieceType pt = getPieceTypeAt(index, c);
            result ^= Zobrist<V>::getZHash(index, c, pt);
        }
    }
    return result;
}

}  // namespace hexchess::core

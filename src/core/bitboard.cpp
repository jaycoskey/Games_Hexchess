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

#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include "bitboard.h"
#include "geometry.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"
#include "zobrist.h"


using std::cout;
using std::ostringstream;

using std::set;
using std::string;


namespace hexchess::core {

// Note: Does not check prior status of Bitboard
template<>
void Bitboard<Glinski>::setPiece(Index index, Color c, PieceType pt, bool value) {
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
        throw std::logic_error("Bitboard<Glinski>::setPiece: Unrecognized piece type");
    }
}

template<>
void Bitboard<Glinski>::addPiece(Index index, Color c, PieceType pt) {
    setPiece(index, c, pt, true);
}

template<>
void Bitboard<Glinski>::removePiece(Index index, Color c, PieceType pt) {
    setPiece(index, c, pt, false);
}


// ========================================
// Get Color and PieceType

template<>
Color Bitboard<Glinski>::getColorAt(Index index) const {
    assert(anyPieceBits().test(index));
    return anyPieceBits(Color::Black).test(index)
        ? Color::Black
        : Color::White;
}

// Adding one more level to the decision tree (e.g., testing non-Pawns for isBN) doesn't pay off.
template<>
PieceType Bitboard<Glinski>::getPieceTypeAt(Index index, Color c) const {
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
            throw std::logic_error("Inconsistent bitboard information on cell #" + std::to_string(index));
        }
        return PieceType::King;
    }
}

template<>
PieceType Bitboard<Glinski>::getPieceTypeAt(Index index) const {
    switch(getColorAt(index)) {
    case Color::Black:
        return getPieceTypeAt(index, Color::Black);
    case Color::White:
        return getPieceTypeAt(index, Color::White);
    default:
        throw std::logic_error("Bitboard<Glinski>::getPieceTypeAt: Unrecognized Color");
    }
}

// ========================================
// Constructor

template<>
Bitboard<Glinski>::Bitboard(bool doPopulate)
  : _anyPieceBits{},
    _colorToAnyPieceBits{ { Color::Black, {}}, {Color::White, {}} },
    _colorToKingBits{     { Color::Black, {}}, {Color::White, {}} },
    _colorToQueenBits{    { Color::Black, {}}, {Color::White, {}} },
    _colorToRookBits{     { Color::Black, {}}, {Color::White, {}} },
    _colorToBishopBits{   { Color::Black, {}}, {Color::White, {}} },
    _colorToKnightBits{   { Color::Black, {}}, {Color::White, {}} },
    _colorToPawnBits{     { Color::Black, {}}, {Color::White, {}} },
    _enPassantBits{}
{
    for (const PiecePlacement& pp : Glinski::piecePlacements()) {
        const auto [index, c, pt] = pp;
        addPiece(index, c, pt);

        assert(getColorAt(index) == c);
        assert(getPieceTypeAt(index) == pt);
        assert(getPieceTypeAt(index, c) == pt);
    }
}

// ========================================

template<>
template<class OutputMoveIt>
void Bitboard<Glinski>::getLeapMoves(
    OutputMoveIt moves_first,
    Index from, Color mover,
    PieceType pt, Indices dests
    ) const
{
    // TODO: Implement method
    throw NotImplementedException{};
}

template<>
template<class OutputMoveIt>
void Bitboard<Glinski>::getStandardPawnMoves(
    OutputMoveIt moves_first,
    Index from,
    Color mover
    ) const
{
    // TODO: Implement method
    throw NotImplementedException{};

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
void Bitboard<Glinski>::getSlideMoves(
    OutputMoveIt moves_first,
    Index from,
    Color mover,
    PieceType pt,
    const HexRays<Glinski>& rays
    ) const
{
    // TODO: Implement method
    throw NotImplementedException{};
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
const string Bitboard<Glinski>::bitboard_string() /* const */ {
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
const string Bitboard<Glinski>::fen_string() /* const */ {
    ostringstream oss;

    int blankCount = 0;
    int rowNum = 0;
    int cellsRemainingInRow = Glinski::fenRowLengths[rowNum];
    for (Index index : Glinski::fenOrderToIndex) {
        if (isEmpty(index)) {
            blankCount++;
        } else {
            if (blankCount > 0) {
                oss << blankCount;
                blankCount = 0;
            }
            Color c = getColorAt(index);
            PieceType pt = getPieceTypeAt(index, c);
            oss << ::hexchess::core::fen_string(c, pt);
        }
        cellsRemainingInRow--;

        if (cellsRemainingInRow == 0) {
            if (blankCount > 0) {
                oss << blankCount;
                blankCount = 0;
            }
            rowNum++;
            if (rowNum < Glinski::ROW_COUNT) {
                oss << '/';
                cellsRemainingInRow = Glinski::fenRowLengths[rowNum];
            }
        }
    }
    return oss.str();
}

template<>
const string Bitboard<Glinski>::piecebits_string() /* const */ {
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

template<>
template<class OutputMoveIt>
void Bitboard<Glinski>::getPseudoLegalMoves(OutputMoveIt moves_first,
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
        throw std::logic_error("Bitboard: pseudoLegalDestinations: Unrecognized PieceType");
    }
}

template<>
template<class OutputMoveIt>
void Bitboard<Glinski>::getPseudoLegalMoves(OutputMoveIt moves_first, Color c) const {
    // TODO: Implement method
}

template<>
template<class OutputMoveIt>
void Bitboard<Glinski>::getLegalMoves(OutputMoveIt moves_first, Color c) const {
    // TODO: Implement method
}

template<>
ZHash Bitboard<Glinski>::zobristHash() const {
    ZHash result = 0;
    for (Index index = 0; index < V::CELL_COUNT; ++index) {
        if (!isEmpty(index)) {
            Color c = getColorAt(index);
            PieceType pt = getPieceTypeAt(index, c);
            result ^= Zobrist<Glinski>::getZHash(index, c, pt);
        }
    }
    return result;
}

}  // namespace hexchess::core

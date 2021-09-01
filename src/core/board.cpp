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
#include <utility>

#include "board.h"
#include "fen.h"
#include "geometry.h"
#include "move.h"
#include "player.h"
#include "util.h"
#include "util_hexchess.h"
#include "variant.h"
#include "zobrist.h"

using std::cout;
using std::ostringstream;
using std::string;

using std::pair;
using std::set;
using std::vector;


namespace hexchess::core {

using player::Player;

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
    // \todo: Implement castling
    // _colorToRookCastlingAvailabilityBits
    //                     { { Color::Black, {}}, {Color::White, {}} },

    _optEpIndex{std::nullopt}
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

            // assert(getColorAt(index) == c);
            // assert(getPieceTypeAt(index) == pt);
            // assert(getPieceTypeAt(index, c) == pt);

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
{ /* TODO */ }

// ========================================
// Fundamental operations

template<>
void Board<Glinski>::clear() {
    _anyPieceBits.reset();

    for (Color c : {Color::Black, Color::White}) {
        _colorToAnyPieceBits[c].reset();
        _colorToKingBits[c].reset();
        _colorToQueenBits[c].reset();
        _colorToRookBits[c].reset();
        _colorToBishopBits[c].reset();
        _colorToKnightBits[c].reset();
        _colorToPawnBits[c].reset();
    }
    _optEpIndex = std::nullopt;
    /// \todo: Support castling: Have Board<>::clear() modfy _castlingBits
}

template<>
void Board<Glinski>::reset(bool doPopulate) {
    clear();
    if (doPopulate) {
        Fen<V> fenInitial{Glinski::fenInitial};
        initialize(fenInitial);
    }
}

// ========================================
// Non-piece data

template<>
HalfMoveCounter Board<Glinski>::currentCounter() const {
    return _currentCounter;
}

// ========================================
// Write piece data
// (See header)

// ========================================
// Piece movement capabilities (stored as Bits and Indices)
// (See header)

// ========================================
// Read piece data
// (See header)

// ========================================
// Piece index queries
// (See header)

// ========================================
// Piece counts
// (See header)

// ========================================
// Other piece location methods

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
    return getPieceTypeAt(index, getColorAt(index));
}

template<>
PiecesDense Board<Glinski>::piecesDense() const {
    PiecesDense result{};

    for (Index index = 0; index < V::CELL_COUNT; ++index) {
        if (isPieceAt(index)) {
            Color c = getColorAt(index);
            PieceType pt = getPieceTypeAt(index, c);
            result.push_back(std::make_tuple(index, c, pt));
        }
    }
    return result;
}

/// \todo Refactor both signatures of piecesDense to remove code duplication.
template<>
PiecesDense Board<Glinski>::piecesDense(Color c) const {
    PiecesDense result{};

    for (Index index = 0; index < V::CELL_COUNT; ++index) {
        if (isPieceAt(index)) {
            if (getColorAt(index) == c) {
                PieceType pt = getPieceTypeAt(index, c);
                result.push_back(std::make_tuple(index, c, pt));
            }
        }
    }
    return result;
}

template<>
PiecesSparse Board<Glinski>::piecesSparse() const {
    PiecesSparse result{};

    for (Index index = 0; index < V::CELL_COUNT; ++index) {
        if (isPieceAt(index)) {
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
// Board hashing

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

template<>
bool Board<Glinski>::isRepetition() const {
    throw NotImplementedException{};
}

// ========================================
// String methods

template<>
const string Board<Glinski>::board_bits_string() /* const */ {
    ostringstream oss;

    oss << "Any Piece : " << reved(_anyPieceBits.to_string())  << "\n";
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
const string Board<Glinski>::board_pgn_string() const {
    std::ostringstream oss;

    oss << "\n";
    for (Size k = 0; k < _moveStack.size(); ++k) {
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

// ========================================
// Finding, getting, and caching moves

template<>
void Board<Glinski>::recordObstructedHexRayCore(
    Index obsIndex, Index rayStart, const HexDir& rayDir) const
{
    if (!_cache.obstructedHexRayMap.contains(obsIndex)) {
        _cache.obstructedHexRayMap[obsIndex] = HexRayCores{};
    }
    _cache.obstructedHexRayMap[obsIndex].push_back(mkPair(rayStart, rayDir));
}

template<>
const HexRayCores& Board<Glinski>::getObstructedHexRayCores(Index obsIndex) const {
    if (_cache.obstructedHexRayMap.contains(obsIndex)) {
        // JMC: cout << "Board::getObstructedHexRayCores: returning " << _cache.obstructedHexRayMap.at(obsIndex).size() << " cores\n";
        return _cache.obstructedHexRayMap.at(obsIndex);
    } else {
        // JMC: cout << "Board::getObstructedHexRayCores: returning no cores\n";
        HexRayCores result{};
        // cout << "Board::getObstructedHexRayCores: no cores means " << result.size() << " cores\n";
        cout << "";
        return result;
    }
}

template<>
void Board<Glinski>::findLeapMoves(/* out */ Moves& moves,
    Index from, Color mover, PieceType pt, const Indices& dests
    ) const
{
    transform_push_back_if(dests.begin(), dests.end(), moves,
        [=](Index dest) { return !isPieceAt(dest, mover); },  // Cannot capture own piece
        [&](Index dest) {
            return Move { mover, pt, from, dest,
                          MoveEnum::Simple,
                          isPieceAt(dest, opponent(mover))  // Not a Pawn promotion
                              ? std::make_optional(getPieceTypeAt(dest, opponent(mover)))
                              : std::nullopt,
                          std::nullopt,  // Not a Pawn promotion
                          std::nullopt
                        };
            }
        );
}

template<>
void Board<Glinski>::findSlideMoves(
    /* out */ Moves& moves,
    Index from,
    Color mover,
    PieceType pt,
    const HexRays<Glinski>& rays
    ) const
{
    for (const HexRay<V>& ray : rays) {
        for (Index dest : ray.indices()) {
            if (isPieceAt(dest)) {
                recordObstructedHexRayCore(dest, ray.start(), ray.dir());
                Color destColor = getColorAt(dest);
                if (destColor == mover) {
                    break;
                }
            }
            Move move{ mover, pt, from, dest,
                       MoveEnum::Simple,
                       isPieceAt(dest, opponent(mover))
                           ? std::make_optional(getPieceTypeAt(dest, opponent(mover)))
                           : std::nullopt,
                       std::nullopt,  // Not a Pawn promotion
                       std::nullopt  // Unknown Check type
                     };
            moves.push_back(move);
            break;
        }
    }
}
/// \brief Find available (non-en-passant) Pawn moves. For Pawn promotion scenarios,
///        create multiple moves, one for each Pawn promotion type.
///
/// Note: Some of this code makes variant-specific presumptions about what types of moves
///       Pawns can make. This would need to be revised for some future variants.
/// \todo: Consider refactoring duplicated code into one or more new helper methods.
template<>
void Board<Glinski>::findStandardPawnMoves(
    /* out */ Moves& moves,
    Index from,
    Color mover
    ) const
{
    // ========== Can advance? ==========
    // ---------- Can advance one space? ----------
    for (Index adv1Index : V::pawnAdvance1Indices(from, mover)) {
        if (!isPieceAt(adv1Index)) {
            if (V::pawnPromotionBits(mover).test(adv1Index)) {
                for (PieceType promotionPieceType : V::promotionPieceTypes) {
                    Move move{ mover, PieceType::Pawn, from, adv1Index,
                           MoveEnum::PawnPromotion,
                           std::nullopt,  // Not a capture
                           std::make_optional<PieceType>(promotionPieceType),
                           std::nullopt  // Check type not yet determined
                         };
                    moves.push_back(move);
                }
            } else {
                Move move{ mover, PieceType::Pawn, from, adv1Index,
                    MoveEnum::Simple,
                    std::nullopt,  // Not a capture
                    std::nullopt,  // Not a Pawn promotion
                    std::nullopt   // Check type not yet determined
                };
                moves.push_back(move);
            }

            // Having confirmed that the first space is clear, check to see
            //     whether any Pawn double-step moves are available.
            // ---------- Can advance two spaces? ----------
            if (V::pawnStartBits(mover).test(from)) {
                for (Index adv2Index : V::pawnAdvance2Indices(from, mover)) {
                    if (!isPieceAt(adv2Index)) {
                        Move move{ mover, PieceType::Pawn, from, adv2Index,
                                   MoveEnum::Simple,
                                   std::nullopt,  // Not capture
                                   std::nullopt,  // Not Pawn promotion
                                   std::nullopt   // Unknown Check type
                                 };
                        moves.push_back(move);
                    }
                }
            }
        }
    }
    // ========== Can capture? ==========
    for (Index capIndex : V::pawnCaptureIndices(from, mover)) {
        if (isPieceAt(capIndex, opponent(mover))) {
            if (V::pawnPromotionBits(mover).test(capIndex)) {  // Pawn promotion?
                for (PieceType promotionPieceType : V::promotionPieceTypes) {
                    Move move{ mover, PieceType::Pawn, from, capIndex,
                        MoveEnum::PawnPromotion,
                        std::nullopt, // Capture
                        std::make_optional<PieceType>(promotionPieceType), // Pawn promotion
                        std::nullopt  // Check
                        };
                    moves.push_back(move);
                }
            } else {
                Move move{ mover, PieceType::Pawn, from, capIndex,
                        MoveEnum::Simple,
                        std::nullopt,  // Not a capture
                        std::nullopt,  // Not a Pawn promotion
                        std::nullopt   // Unknown Check type
                        };
                moves.push_back(move);
            }
        }
    }
}

template<>
void Board<Glinski>::findPseudoLegalMoves(/* out */ Moves& moves,
    Index from, Color mover, PieceType pt
    ) const
{
    switch (pt) {
    case PieceType::King:
        findLeapMoves(moves, from, mover, pt, V::kingDests.at(from));
        break;
    case PieceType::Queen:
        findSlideMoves(moves, from, mover, pt, V::queenRays.at(from));
        break;
    case PieceType::Rook:
        findSlideMoves(moves, from, mover, pt, V::rookRays.at(from));
        break;
    case PieceType::Bishop:
        findSlideMoves(moves, from, mover, pt, V::bishopRays.at(from));
        break;
    case PieceType::Knight:
        findLeapMoves(moves, from, mover, pt, V::knightDests.at(from));
        break;
    case PieceType::Pawn:
        findStandardPawnMoves(moves, from, mover);
        break;
    default:
        throw std::logic_error("Board: pseudoLegalDestinations: Unrecognized PieceType");
    }
}

template<>
void Board<Glinski>::findPseudoLegalMoves(/* out */ Moves& moves, Color c) const {
    for (auto [from, c, pt] : piecesDense(c)) {
        findPseudoLegalMoves(moves, from, c, pt);
    }
}

template<>
void Board<Glinski>::recordPseudoLegalMoves(const Moves& moves) const {
    _cache.optPseudoLegalMoves = std::make_optional<Moves>(moves);
};

template<>
Moves Board<Glinski>::getPseudoLegalMoves(Color c) const {
    if (_cache.optPseudoLegalMoves.has_value()) {
        return _cache.optPseudoLegalMoves.value();
    } else {
        Moves moves{};
        findPseudoLegalMoves(moves, c);
        recordPseudoLegalMoves(moves);
        return moves;
    }
}

// ----------------------------------------

/// \brief Returns true iff a player moving a piece from \p from to \p to
///        would cause put that same player's King in check.
///
/// The only types of moves that can put the moving player into (self-)check is
///   one that removes an obstruction from an opponent's sliding piece.
template<>
bool Board<Glinski>::isOwnKingAttackedAfterOwnMove(Color mover, Index from, Index to) const
{
    (void) to;  // Could check whether piece has moved out from the path between obstruction & King
    // cout << "JMC: isOwnKingAttackedAfterOwnMove: Entering: mover=" << mover << "; from=" << from << "; to=" << to << "\n";
    Index kIndex = getKingIndex(mover);
    // cout << "JMC: isOwnKingAttackedAfterOwnMove: Calling getObstructedHexRayCores with from=" << from << "\n";
    const HexRayCores& obstructed = getObstructedHexRayCores(from);
    if (obstructed.size() == 0) {
        return false;
    }
    // cout << "JMC: isOwnKingAttackedAfterOwnMove: Called getObstructedHexRayCores. Result ('obstructed') contains " << obstructed.size() << " HexRayCores\n";
    for (const auto& [obsStart, obsDir] : obstructed) {
        // cout << "JMC: isOwnKingAttackedAfterOwnMove: obsStart=" << obsStart << "; obsDir=" << obsDir << "\n";
        if (obsStart == from) {
            // cout << "JMC: isOwnKingAttackedAfterOwnMove: Found an obstruction from " << from << "\n";
            // The piece moved was a slider obstruction.
            HexDir obsToKingDir = (V::indexToPos(kIndex) - V::indexToPos(obsStart));
            Short collinearTestValue = obsDir.hex0 * obsToKingDir.hex1
                                     - obsDir.hex1 * obsToKingDir.hex0;
            if (collinearTestValue == 0) {
                // cout << "JMC: \tisOwnKingAttackedAfterOwnMove: Passed collinear test\n";
                // They're collinear, so this Ray might now attack the King. Re-trace it.
                for (HexPos cursor = V::indexToPos(obsStart) + obsDir;
                    V::isOnBoard(cursor);
                    cursor += obsDir)
                {
                    // cout << "JMC: \tisOwnKingAttackedAfterOwnMove: Tracing " << obsDir << "; at cursor=" << cursor << "\n";
                    Index curIndex = V::posToIndex(cursor);
                    if (curIndex == kIndex) {
                        return true;
                    }
                    if (curIndex == to || isPieceAt(curIndex)) {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}

template<>
void Board<Glinski>::findLegalMoves(/* out */ Moves& legalMoves,
    Color c, const Moves& pseudoLegalMoves) const
{
    for (const Move& move : pseudoLegalMoves) {
        if (!isOwnKingAttackedAfterOwnMove(c, move.from(), move.to())) {
            legalMoves.push_back(move);
        }
    }
}

template<>
void Board<Glinski>::recordLegalMoves(const Moves& moves) const {
    _cache.optLegalMoves = std::make_optional<Moves>(moves);
};

template<>
Moves Board<Glinski>::getLegalMoves(Color c) const {
    if (_cache.optLegalMoves.has_value()) {
        return _cache.optLegalMoves.value();
    } else {
        Moves pseudoLegalMoves = getPseudoLegalMoves(c);
        Moves result{};
        findLegalMoves(result, c, pseudoLegalMoves);
        recordPseudoLegalMoves(result);
        return result;
    }
}

/// \todo Refactor both signatures of piecesDense to remove code duplication.
template<>
bool Board<Glinski>::isAttacked(Index tgtIndex, Color tgtColor) const {
    for (const Move& move : getLegalMoves(opponent(tgtColor))) {
        if (move.to() == tgtIndex) {
            return true;
        }
    }
    return false;
}

template<>
bool Board<Glinski>::isAttacking(Index from, Color, PieceType pt, Index tgt) const {
    throw NotImplementedException{"Board<>::isAttacking"};
}

// ----------------------------------------

template<>
void Board<Glinski>::recordCheckEnum(CheckEnum checkEnum) const {
    _cache.optCheckEnum = std::make_optional<CheckEnum>(checkEnum);
};

template<>
CheckEnum Board<Glinski>::getCheckEnum() const {
    if (_cache.optCheckEnum.has_value()) {
        return _cache.optCheckEnum.value();
    }
    OptGameOutcome optOutcome = _cache.optOutcome;
    if (optOutcome.has_value()
        && optOutcome.value().termination() != Termination::None)
    {
        if (optOutcome.value().termination() == Termination::Win_Checkmate) {
            recordCheckEnum(CheckEnum::Checkmate);
            return CheckEnum::Checkmate;
        } else {
            recordCheckEnum(CheckEnum::None);
            return CheckEnum::None;
        }
    }
    if (isAttacked(getKingIndex(mover()), mover())) {
        recordCheckEnum(CheckEnum::Check);
        return CheckEnum::Check;
    }
    recordCheckEnum(CheckEnum::None);
    return CheckEnum::None;
}

// ----------------------------------------

template<>
void Board<Glinski>::recordOutcome(const GameOutcome& gameOutcome) const {
    _cache.optOutcome = std::make_optional<GameOutcome>(gameOutcome);
};

/// Note: This method test for all forms of Outcome that result from board position.
///       It does not test for Resignation or Draw by agreement.
///       If a game ends via either of those condiions, then we should not reach here.
template<>
OptGameOutcome Board<Glinski>::getOptOutcome() const {
    // ---------- Has the Outcome already been cached? ----------
    if (_cache.optOutcome.has_value()) {
        return _cache.optOutcome.value();
    }
    // ---------- Checkmate or Stalemate ----------
    if (getLegalMoves(mover()).size() == 0) {
        if (isAttacked(getKingIndex(mover()), mover())) {
            GameOutcome outcome{Termination::Win_Checkmate, opponent(mover())};
            recordOutcome(outcome);
            return outcome;
        } else {
            GameOutcome outcome{Termination::Draw_Stalemate, opponent(mover())};
            recordOutcome(outcome);
            return outcome;
        }
    }

    // ----------  Insufficient resources ----------
    // See pp. 79-82 of Glinski's book First Theories of Hexagonal Chess,
    //     and McCooey's Endgame analysis for Hexagonal Chess.
    Short pieces = pieceCount();
    if (pieces == 2) {
        GameOutcome outcome{Termination::Draw_InsufficientResources};
        recordOutcome(outcome);
        return outcome;
    }
    if (pieces == 3) {
        // If the non-King piece is a Bishop or Knight,
        //     then there are insufficient resources.
        for (auto [index, c, pt] : piecesDense()) {
            if (pt != PieceType::King) {
                if (pt == PieceType::Bishop || pt == PieceType::Knight) {
                    GameOutcome outcome{Termination::Draw_InsufficientResources};
                    recordOutcome(outcome);
                    return _cache.optOutcome;
                } else {
                    break;  // Not insufficient resources
                }
            }
        }
    }

    // ---------- Draw by 3x Board repetition ----------
    if (_zHashToCounters.at(_zHashes.at(_currentCounter)).size() == 3) {
        GameOutcome outcome{Termination::Draw_3xBoardRepetition};
        recordOutcome(outcome);
        return outcome;
    }
    // ---------- Draw by 50 Move rule ----------
    if (_nonProgressCounter >= 50) {
        GameOutcome outcome{Termination::Draw_50MoveRule};
        recordOutcome(outcome);
        return _cache.optOutcome;
    }

    // Cached value for game in progress must differ from that representing an emtpy cache).
    GameOutcome outcome{Termination::None};
    recordOutcome(outcome);
    return _cache.optOutcome;
}

template<>
GameOutcome Board<Glinski>::getOutcome() const {
    assert(getOptOutcome().has_value());
    return getOptOutcome().value();
}

// ----------------------------------------

template<>
bool Board<Glinski>::getIsGameOver() const {
    return !getOptOutcome().has_value();
}

// ========================================

template<>
void Board<Glinski>::_moveBit(map<Color, typename Glinski::Bits>& bits,
    Color mover, Index from, Index to)
{
    bits[mover].reset(from);
    bits[mover].set(to);
}

template<>
void Board<Glinski>::_moveBit(typename Glinski::Bits& bits,
    Index from, Index to)
{
    bits.reset(from);
    bits.set(to);
}

/// \todo Return bool or std::optional<Termination>?
template<>
void Board<Glinski>::moveExec(Move& move) {
    // Update bitboards
    _moveBit(_anyPieceBits, move.from(), move.to());
    _moveBit(_colorToAnyPieceBits, move.mover(), move.from(), move.to());

    switch (move.pieceType()) {
    case PieceType::King:
        _moveBit(_colorToKingBits, move.mover(), move.from(), move.to());
        _colorToKingIndex[move.mover()] = move.to();
        break;
    case PieceType::Queen:
        _moveBit(_colorToQueenBits, move.mover(), move.from(), move.to());
        break;
    case PieceType::Rook:
        _moveBit(_colorToRookBits, move.mover(), move.from(), move.to());
        break;
    case PieceType::Bishop:
        _moveBit(_colorToBishopBits, move.mover(), move.from(), move.to());
        break;
    case PieceType::Knight:
        _moveBit(_colorToKnightBits, move.mover(), move.from(), move.to());
        break;
    case PieceType::Pawn:
        _moveBit(_colorToPawnBits, move.mover(), move.from(), move.to());
        break;
    default:
        throw std::logic_error("Board: moveMake: Unrecognized PieceType");
    }

    // TODO: Add support for castling
    // When a Rook is moved from index, and this Bits tests true at index, then that bit should be cleared.
    // When a King is moved, this Bits value should be cleared.
    // std::map<Color, typename V::Bits> _colorToRookCastlingAvailabilityBits;
    if (abs(V::row(move.to() - move.from())) == 4) {  // 4 half-rows = 2 rows
        // One step forward from Pawn's starting position.
        // Note: Taking the first element of the list of advance positions is Variant-specific.
        Index epInd = V::colorToPawnAdvance1Indices.at(move.mover()).at(move.from())[0];
        _optEpIndex = std::make_optional(epInd);
    } else {
        _optEpIndex = std::nullopt;
    }

    // Update history: Progress counter
    if (move.isProgressMove()) {
        _nonProgressCounter = 0;
    } else {
        _nonProgressCounter++;
    }
    _nonProgressCounters.push_back(_currentCounter);

    // Update history: Zobrist Hash info
    std::map<ZHash, std::vector<HalfMoveCounter>> _zHashToCounters{};  // To track repeats
    ZHash hash = zobristHash();
    if (_zHashToCounters.contains(hash)) {
        _zHashToCounters[hash] = Shorts{1, _currentCounter};
    } else {
        _zHashToCounters[hash].push_back(_currentCounter);
        if (_zHashToCounters[hash].size() == 3) {
           _cache.optOutcome = std::make_optional<GameOutcome>(Termination::Draw_3xBoardRepetition);
        }
    }
    _zHashes.push_back(hash);

    // Update history: Move stack
    _moveStack.push_back(move);

    // Update HalfMoveCounter and mover Color
    _currentCounter += 1;
    _mover = opponent(move.mover());

    // ---------- Cache Outcome ----------
    _cache.optOutcome = getOptOutcome();  // Necessary to determine whether Game is over.
    if (isGameOver()) {
        string msg = getOptOutcome().value().game_outcome_reader_string(move.mover());
        cout << "Game is over: " << msg << "\n";
    }

    // ---------- Cache Pseudo-legal and legal moves ----------
    (void) getLegalMoves(_mover);
}

// void moveRedo(const Move& move) {
//     throw NotImplementedException{"Move::moveRedo"};
// }

// void moveUndo( const Move& move) {
//     throw NotImplementedException{"Move::moveUndo"};
// }

// ========================================
// Reading and writing game state

template<>
const Indices& Board<Glinski>::attackers(Index tgtInd, const Moves& moves) const {
    Indices result{};

    for (const Move& move : moves) {
        if (move.to() == tgtInd) { result.push_back(move.to()); }
    }
    return result;
}

// template<>
// bool Board<Glinski>::causesCheck(const Move& move) {
//     throw NotImplementedException{"Board<Glinski>::causesCheck(Color)"};
// }

template<>
bool Board<Glinski>::isCheck() const {
    return getCheckEnum() == CheckEnum::Check;
}

template<>
bool Board<Glinski>::isCheckmate() const {
    return isGameOver() && getOptOutcome().value().termination() == Termination::Win_Checkmate;
}

template<>
bool Board<Glinski>::isDrawByInsufficientResources() const {
    throw NotImplementedException{"Board<>::isDrawByInsufficientResources"};
}

template<>
bool Board<Glinski>::isDrawByStalemate() const {
    throw NotImplementedException{"Board<>::isDrawByStalemate"};
}

/// \todo Impement Board<>::isPinned for Board evaluation?
// template<>
// bool Board<Glinski>::isPinned(Index tgtInd, Color c) const {
//     throw NotImplementedException{"Board<Glinski>::isPinned"};
// }

/// \todo Impement Board<>::pinningIndices for Board evaluation?
// template<>
// Indices Board<Glinski>::pinnningIndices(Index tgtInd, Color c) const {
//     throw NotImplementedException{"Board<Glinski>::pinningIndides"};
// }
template<>
void Board<Glinski>::setMoveCheckEnum(Move& move) {
    throw NotImplementedException{};
    // Idea:
    // Move move = Move(move);
    // Index kIndex = getKingIndex(move.mover);
    // Test for Check and Checkmate
    // moveUndo(move);
}

}  // namespace hexchess::core

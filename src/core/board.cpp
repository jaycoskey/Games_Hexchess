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

#include <map>
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

using std::cerr;
using std::cout;
using std::ostringstream;
using std::string;

using std::map;
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
void Board<Glinski>::addPiece(Index index, Color c, PieceType pt) {
    setPiece(index, c, pt, true);
}

template<>
void Board<Glinski>::removePiece(Index index, Color c, PieceType pt) {
    setPiece(index, c, pt, false);
}

template<>
void Board<Glinski>::movePiece(Index from, Index to, Color c, PieceType pt) {
    setPiece(from, c, pt, false);
    setPiece(to, c, pt, true);
}

template<>
void Board<Glinski>::changePieceType(Index index, Color c, PieceType ptOld, PieceType ptNew) {
    setPiece(index, c, ptOld, false);
    setPiece(index, c, ptNew, true);
}

template<>
HalfMoveCounter Board<Glinski>::currentCounter() const {
    return _currentCounter;
}

template<>
void Board<Glinski>::initialize(const Fen<Glinski>& fen) {
    Scope scope("Board::initialize", true);
    print(std::cout, scope(), "Board=", name(), ", counter=", currentCounter(),
        ". Fen=", fen.fen_string(), "\n");
    // (1) Piece placement
    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        if (fen.piecesSparse[index].has_value()) {
            auto [c, pt] = fen.piecesSparse[index].value();
            addPiece(index, c, pt);
            if (pt == PieceType::King) {
                setKingIndex(index, c);
            }
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
Board<Glinski>::Board(const string& name, bool doPopulate)
    : _name{name}
{
    _anyPieceBits = V::Bits{};
    _colorToAnyPieceBits = map<Color, typename V::Bits> {
        { Color::Black, {}}, {Color::White, {}} };
    _colorToKingBits     = map<Color, typename V::Bits> {
        { Color::Black, {}}, {Color::White, {}} };
    _colorToQueenBits    = map<Color, typename V::Bits> {
        { Color::Black, {}}, {Color::White, {}} };
    _colorToRookBits     = map<Color, typename V::Bits> {
        { Color::Black, {}}, {Color::White, {}} };
    _colorToBishopBits   = map<Color, typename V::Bits> {
        { Color::Black, {}}, {Color::White, {}} };
    _colorToKnightBits   = map<Color, typename V::Bits> {
        { Color::Black, {}}, {Color::White, {}} };
    _colorToPawnBits     = map<Color, typename V::Bits> {
        { Color::Black, {}}, {Color::White, {}} };

    _colorToKingIndex    = map<Color, Index> { { Color::Black, 12345}, {Color::White, 12345} };
    _optEpIndex          = std::nullopt;

    if (doPopulate) {
        Fen<V> fenInitial{Glinski::fenInitial};
        initialize(fenInitial);
    }
}

/// \brief Constructs Board from Fen instance (derived from FEN string)
template<>
Board<Glinski>::Board(const string& name, const Fen<V>& fen)
    : Board<Glinski>::Board(name, false)
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
                setKingIndex(index, c);
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
Board<Glinski>::Board(const string& name, const string& fenStr)
    : Board<Glinski>::Board{name, Fen<V>{fenStr}}
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
    if (!anyPieceBits().test(index)) {
        ostringstream oss;
        oss << "Board::getColorAt: No piece at " << index;
        string msg = oss.str();
        throw std::logic_error{msg};
    }
    return anyPieceBits(Color::Black).test(index)
        ? Color::Black
        : Color::White;
}

template<>
const string Board<Glinski>::board_bits_string() const {
    ostringstream oss;

    // Header with numbered columns
    oss << "######### : ";
    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        if (index % 10 == 0) {
            oss << index / 10;
        } else {
            oss << " ";
        }
    }
    oss << "\n";
    oss << "######### : ";
    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        oss << index % 10;
    }
    oss << "\n";

    oss << "Any Piece : " << reved(_anyPieceBits.to_string())  << "\n";
    oss << "    Black:\n";
    oss << "\tA:  " << reved(_colorToAnyPieceBits.at(Color::Black).to_string()) << "\n";
    oss << "\tK:  " << reved(_colorToKingBits.at(Color::Black).to_string())     << "\n";
    oss << "\tQ:  " << reved(_colorToQueenBits.at(Color::Black).to_string())    << "\n";
    oss << "\tR:  " << reved(_colorToRookBits.at(Color::Black).to_string())     << "\n";
    oss << "\tB:  " << reved(_colorToBishopBits.at(Color::Black).to_string())   << "\n";
    oss << "\tN:  " << reved(_colorToKnightBits.at(Color::Black).to_string())   << "\n";
    oss << "\tP:  " << reved(_colorToPawnBits.at(Color::Black).to_string())     << "\n";
    oss << "    White:\n";
    oss << "\tA:  " << reved(_colorToAnyPieceBits.at(Color::White).to_string()) << "\n";
    oss << "\tK:  " << reved(_colorToKingBits.at(Color::White).to_string())     << "\n";
    oss << "\tQ:  " << reved(_colorToQueenBits.at(Color::White).to_string())    << "\n";
    oss << "\tR:  " << reved(_colorToRookBits.at(Color::White).to_string())     << "\n";
    oss << "\tB:  " << reved(_colorToBishopBits.at(Color::White).to_string())   << "\n";
    oss << "\tN:  " << reved(_colorToKnightBits.at(Color::White).to_string())   << "\n";
    oss << "\tP:  " << reved(_colorToPawnBits.at(Color::White).to_string())     << "\n";

    return oss.str();
}

template<>
PieceType Board<Glinski>::getPieceTypeAt(Index index, Color c) const {
    if (!anyPieceBits().test(index)) {
        print(cout, "Board=", name(), ", counter=", currentCounter(), ": ", board_bits_string());
    }
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
            throw std::logic_error("Inconsistent board information on cell #"
                + std::to_string(index));
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
const string Board<Glinski>::board_string() const {
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

    oss << std::setw(2) << rowNum << ": ";
    // oss << "indent=" << std::setw(2) << indent(0) << ": "
    oss << std::setw(indent(0)) << " ";
    for (Index index : Glinski::fenOrderToIndex) {
        if (isEmpty(index)) {
            oss << "  --";
        } else {
            Color c = getColorAt(index);
            PieceType pt = getPieceTypeAt(index, c);
            oss << "  " << piece_string(c, pt);
        }
        cellsRemainingInRow--;

        if (cellsRemainingInRow == 0) {
            oss << "\n";
            rowNum++;
            if (rowNum < Glinski::ROW_COUNT) {
                oss << std::setw(2) << rowNum << ": ";
                // oss << "indent=" << std::setw(2) << indent(rowNum) << ": "
                oss << std::setw(indent(rowNum)) << " ";
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
const string Board<Glinski>::moves_pgn_string() const {
    std::ostringstream oss;

    oss << "\n";
    for (Size k = 0; k < _moveStack.size(); ++k) {
        if (k % 2 == 0) {
            oss << _moveStack[k] << ". ";
        } else {
            oss << _moveStack[k] << " ";
        }
        oss << _moveStack[k].move_pgn_string();
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
void Board<Glinski>::_bitsConsistencyTest() const {
    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        Short pieceFoundCount = 0;
        for (Color c : {Color::Black, Color::White}) {
            Short thisColorPieceFoundCount = 0;
            if (_colorToKingBits.at(c).test(index)) {
                thisColorPieceFoundCount++;
            }
            if (_colorToQueenBits.at(c).test(index)) {
                thisColorPieceFoundCount++;
            }
            if (_colorToRookBits.at(c).test(index)) {
                thisColorPieceFoundCount++;
            }
            if (_colorToBishopBits.at(c).test(index)) {
                thisColorPieceFoundCount++;
            }
            if (_colorToKnightBits.at(c).test(index)) {
                thisColorPieceFoundCount++;
            }
            if (_colorToPawnBits.at(c).test(index)) {
                thisColorPieceFoundCount++;
            }

            assert(thisColorPieceFoundCount <= 1);
            if (thisColorPieceFoundCount > 0) {
                assert(_colorToAnyPieceBits.at(c).test(index));
            }
            pieceFoundCount += thisColorPieceFoundCount;
        }
        assert(pieceFoundCount <= 1);
        if (pieceFoundCount > 0) {
            assert(_anyPieceBits.test(index));
        }
    }
}

template<>
void Board<Glinski>::_bitsMove(map<Color, typename Glinski::Bits>& bits,
    Color mover, Index from, Index to)
{
    bits[mover].reset(from);
    bits[mover].set(to);
}

template<>
void Board<Glinski>::_bitsMove(typename Glinski::Bits& bits,
    Index from, Index to)
{
    bits.reset(from);
    bits.set(to);
}

template<>
void Board<Glinski>::_bitsReset(Index index, Color c, PieceType pt) {
    assert(getColorAt(index) == c);
    _anyPieceBits.reset(index);
    _colorToAnyPieceBits[c].reset(index);
    _colorToKingBits[c].reset(index);
    _colorToQueenBits[c].reset(index);
    _colorToRookBits[c].reset(index);
    _colorToBishopBits[c].reset(index);
    _colorToKnightBits[c].reset(index);
    _colorToPawnBits[c].reset(index);
}

// ========================================
// Finding pseudo-legal moves

template<>
void Board<Glinski>::recordObstructedHexRayCore(
    Index obsIndex, Index rayStart, const HexDir& rayDir) cache_const
{
    Scope scope{"Board::recordObstructedHexRayCore"};
    // print(cout, scope(), "Previously cached obstructions: ", _cache.obstructedHexRayMap.size(), "\n");
    // print(cout, scope(), "Adding obstruction @ ", obsIndex, " for ray start=", rayStart, ", dir=", rayDir, "\n");
    assert(rayDir.hex0 != 0 || rayDir.hex1 != 0);
    if (!_cache.obstructedHexRayMap.contains(obsIndex)) {
        _cache.obstructedHexRayMap[obsIndex] = HexRayCores{};
    }
    auto hexRayCore = mkPair(rayStart, rayDir);
    _cache.obstructedHexRayMap[obsIndex].push_back(hexRayCore);
}

template<>
const HexRayCores Board<Glinski>::getObstructedHexRayCores(Index obsIndex) cache_const {
    if (_cache.obstructedHexRayMap.contains(obsIndex)) {
        return _cache.obstructedHexRayMap.at(obsIndex);
    } else {
        // TODO
        return HexRayCores{};
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
    const HexRays<Glinski>& rays,
    bool isVirtual
    ) const
{
    for (const HexRay<V>& ray : rays) {
        bool foundObstruction = false;
        Color obstructionColor = Color::Black;

        for (Index dest : ray.indices()) {
            if (isPieceAt(dest)
                && V::isOnBoard(V::indexToPos(dest)))
            {
                foundObstruction = true;
                if (!isVirtual) {
                    recordObstructedHexRayCore(dest, ray.start(), ray.dir());
                }
                Color destColor = getColorAt(dest);
                if (destColor == mover) {
                    obstructionColor = destColor;
                    break;
                }
            }
            if (!foundObstruction || obstructionColor != mover) {
                Move move{ mover, pt, from, dest,
                        MoveEnum::Simple,
                        isPieceAt(dest, opponent(mover))
                            ? std::make_optional(getPieceTypeAt(dest, opponent(mover)))
                            : std::nullopt,
                        std::nullopt,  // Not a Pawn promotion
                        std::nullopt  // Unknown Check type
                    };
                moves.push_back(move);
            }
            if (foundObstruction) {
                break;
            }
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
        Color oppColor = opponent(mover);
        if (!isPieceAt(capIndex, mover) && isPieceAt(capIndex, oppColor)) {
            // Capture
            PieceType oppPt = getPieceTypeAt(capIndex, oppColor);
            if (V::pawnPromotionBits(mover).test(capIndex)) {  // Pawn promotion?
                for (PieceType promotionPieceType : V::promotionPieceTypes) {
                    Move move{ mover, PieceType::Pawn, from, capIndex,
                               MoveEnum::PawnPromotion,
                               oppPt, // Capture
                               std::make_optional<PieceType>(promotionPieceType), // Promotion
                               std::nullopt  // Unknown check type
                             };
                    moves.push_back(move);
                }
            } else /* No PawnPromotion */ {
                Move move{ mover, PieceType::Pawn, from, capIndex,
                           MoveEnum::Simple,
                           oppPt,         // Capture
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
    Index from, Color mover, PieceType pt, bool isVirtual
    ) const
{
    bool debug{false};
    if (debug && !isVirtual) {
        assert(getPieceTypeAt(from, mover) == pt);
    }
    switch (pt) {
    case PieceType::King:
        findLeapMoves(moves, from, mover, pt, V::kingDests.at(from));
        break;
    case PieceType::Queen:
        findSlideMoves(moves, from, mover, pt, V::queenRays.at(from), isVirtual);
        break;
    case PieceType::Rook:
        findSlideMoves(moves, from, mover, pt, V::rookRays.at(from), isVirtual);
        break;
    case PieceType::Bishop:
        findSlideMoves(moves, from, mover, pt, V::bishopRays.at(from), isVirtual);
        break;
    case PieceType::Knight:
        findLeapMoves(moves, from, mover, pt, V::knightDests.at(from));
        break;
    case PieceType::Pawn:
        findStandardPawnMoves(moves, from, mover);
        break;
    default:
        throw std::logic_error("Board::pseudoLegalDestinations: Unrecognized PieceType");
    }
}

template<>
void Board<Glinski>::findPseudoLegalMoves(/* out */ Moves& moves, Color mover) const {
    for (auto [from, c, pt] : piecesDense(mover)) {
        findPseudoLegalMoves(moves, from, c, pt);
    }
}

template<>
bool Board<Glinski>::_moveSanityCheck(const Move& move) const {
    assert(move.from() >= 0 && move.from() < Glinski::CELL_COUNT);
    assert(move.to() >= 0 && move.to() < Glinski::CELL_COUNT);
    assert(move.from() != move.to());
    assert(getPieceTypeAt(move.from(), move.mover()) == move.pieceType());

    if (move.isCapture()) {
        assert(isPieceAt(move.to(), opponent(move.mover())));
    } else {
        assert(!isPieceAt(move.to()));
    }
    return true;
}

template<>
void Board<Glinski>::recordPseudoLegalMoves(const Moves& moves) cache_const {
    bool debug{true};

    if (debug) {
        for (const Move& move : moves) {
            assert(_moveSanityCheck(move));
        }
    }
    _cache.optPseudoLegalMoves = std::make_optional<Moves>(moves);
};

template<>
Moves Board<Glinski>::getPseudoLegalMoves(Color mover) cache_const {
    if (_cache.optPseudoLegalMoves.has_value()) {
        return _cache.optPseudoLegalMoves.value();
    } else {
        Moves moves{};
        findPseudoLegalMoves(moves, mover);
        recordPseudoLegalMoves(moves);
        return moves;
    }
}

// ========================================
// Legal moves

/// \brief Returns true iff a player moving a piece from \p from to \p to
///        would cause put that same player's King in check.
///
/// Tactic: Re-use information about piece move capabilities already cached.
///
/// The only types of moves that can put the moving player into (self-)check is
///   one that removes an obstruction from an opponent's sliding piece.
/// \todo Support scenario in which e.p. capture exposes King to attack by slider.
template<>
bool Board<Glinski>::_isKingAttackedAfterMove(const Move& move, Color kColor) cache_const
{
    Scope scope{"Board::_isKingAttackedAfterMove"};
    // If the King in question has moved, cached info is not helpful
    assert(move.pieceType() != PieceType::King || move.mover() != kColor);

    Index from = move.from();
    Index to = move.to();
    Color mover  = move.mover();
    PieceType moverType  = move.pieceType();
    Index kIndex = getKingIndex(kColor);
    assert(kIndex >= 0 && kIndex < V::CELL_COUNT);

    const HexRayCores obstructed = getObstructedHexRayCores(from);
    for (const auto& [obsStart, obsDir] : obstructed) {
        // print(cout, scope(),
        //     "[obsStart=", obsStart, " obsDir=", obsDir,
        //     ", move=", move.move_pgn_string(false), "] ",
        //     "Testing obstructed Ray\n");
        assert(obsStart != from);  // A piece doesn't obstruct itself.
        if (getColorAt(obsStart) == opponent(kColor)) {
            // The piece moved was an obstruction of the opponent's slider
            HexDir obsToKingDir = (V::indexToPos(kIndex) - V::indexToPos(obsStart));
            Short collinearTestValue = obsDir.hex0 * obsToKingDir.hex1
                                     - obsDir.hex1 * obsToKingDir.hex0;
            if (collinearTestValue == 0) {

                // They're collinear, so this Ray might now attack the King. Re-trace it.
                for (HexPos cursor = V::indexToPos(obsStart) + obsDir;
                    V::isOnBoard(cursor);
                    cursor += obsDir)
                {
                    Index curIndex = V::posToIndex(cursor);
                    if (curIndex == kIndex) {
                        return true;
                    }
                    if (curIndex == to || isPieceAt(curIndex)) {
                        return false;  // Obstructed
                    }
                }
            }
        }
    }
    if (mover == kColor) {
        // Moving piece is the same color as the King, so it cannot attack
        return false;
    }

    // Does moving piece attack King from new position?
    if (moverType == PieceType::Pawn) {
        if (V::pawnCaptureBits(to, mover).test(kIndex)) {
            return true;
        } else if (!V::pawnPromotionBits(mover).test(to)) {
            return false;
        }
        moverType = move.optPromotedTo().value();
    }
    if (isLeaper(moverType)) {
        switch(moverType) {
        case PieceType::King:
            // TODO: Convert kingDests to use std::set?
            for (Index dest : V::kingDests.at(to)) {
                if (dest == kIndex) {
                    return true;  // Note: Shouldn't happen
                }
            }
            return false;
        case PieceType::Knight:
            // TODO: Convert kingDests to use std::set?
            for (Index dest : V::kingDests.at(to)) {
                if (dest == kIndex) {
                    return true;  // Note: Shouldn't happen
                }
            }
            return false;
        default:
            throw std::logic_error{"Board::_isKingAttackedAfterMove: Unrecognized piece type"};
        }
    }
    assert(isSlider(moverType));
    Moves pseudoLegalMoves{};
    /// \todo Test individual rays instead of calling findPseudoLegalMoves
    /// Note: No need to check legality for checks
    findPseudoLegalMoves(pseudoLegalMoves, to, mover, moverType, true);
    for (const Move& move : pseudoLegalMoves) {
        if (move.to() == kIndex) {
            return true;
        }
    }
    return false;
}

template<>
void Board<Glinski>::recordMoveCheckEnum(const Move& move, CheckEnum ce) cache_const {
    MHash hash = move.getHash();
    _cache.mhashToCheckEnum[hash] = ce;
}

template<>
CheckEnum Board<Glinski>::getMoveCheckEnum(const Move& move) cache_const {
    MHash hash = move.getHash();
    if (_cache.mhashToCheckEnum.contains(hash)) {
        return _cache.mhashToCheckEnum.at(hash);
    }
    CheckEnum result = _isKingAttackedAfterMove(move, opponent(move.mover()))
                            ? CheckEnum::Check
                            : CheckEnum::None;
    _cache.mhashToCheckEnum[hash] = result;
    return result;
}

template<>
void Board<Glinski>::recordLegalMoves(const Moves& moves) cache_const {
    bool debug{true};

    if (debug) {
        for (const Move& move : moves) {
            assert(_moveSanityCheck(move));
        }
    }
    _cache.optLegalMoves = std::make_optional<Moves>(moves);
};

// TODO: Also handle Checkmate
template<>
const Moves Board<Glinski>::getLegalMoves(Color c) const {
    assert(c == mover());
    if (_cache.optLegalMoves.has_value()) {
        return _cache.optLegalMoves.value();
    } else {
        Moves pseudoLegalMoves = getPseudoLegalMoves(c);
        Moves result{};
        findLegalMoves(result, c, pseudoLegalMoves);
        recordLegalMoves(result);
        Short legalMoveCount = result.size();
        return result;
    }
}

/// \brief Returns highest level of Check encountered: None -> Check -> Checkmate
///
/// Note: This method calls Board::getMoveCheckEnum, which uses a cache
template<>
CheckEnum Board<Glinski>::setLegalMoveCheckEnums(Color mover) cache_const {
    CheckEnum result = CheckEnum::None;

    for (const Move& move : getLegalMoves(mover)) {
        CheckEnum ce = getMoveCheckEnum(move);
        if (ce == CheckEnum::Checkmate || (result == CheckEnum::None && ce == CheckEnum::Check)) {
            result = ce;
        }
    }
    return result;
}

// ========================================
// Attacks

template<>
bool Board<Glinski>::isAttacking(Index from, Color, PieceType pt, Index tgt) const {
    throw NotImplementedException{"Board<>::isAttacking"};
}

template<>
void Board<Glinski>::recordCheckEnum(CheckEnum checkEnum) cache_const {
    _cache.optCheckEnum = std::make_optional<CheckEnum>(checkEnum);
};

template<>
CheckEnum Board<Glinski>::getCheckEnum() cache_const {
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
    if (isOwnCellAttacked(getKingIndex(mover()))) {
        recordCheckEnum(CheckEnum::Check);
        return CheckEnum::Check;
    }
    recordCheckEnum(CheckEnum::None);
    return CheckEnum::None;
}

template<>
bool Board<Glinski>::isCheck() cache_const {
    return getCheckEnum() == CheckEnum::Check;
}

template<>
void Board<Glinski>::recordOutcome(const GameOutcome& gameOutcome) cache_const {
    _cache.optOutcome = std::make_optional<GameOutcome>(gameOutcome);
};

/// Note: This method test for all forms of Outcome that result from board position.
///       It does not test for Resignation or Draw by agreement.
///       If a game ends via either of those condiions, then we should not reach here.
template<>
GameOutcome Board<Glinski>::getOutcome() cache_const {
    // ---------- Has the Outcome already been cached? ----------
    if (_cache.optOutcome.has_value()) {
        return _cache.optOutcome.value();
    }
    // ---------- Checkmate or Stalemate ----------
    if (getLegalMoves(mover()).size() == 0) {
        if (isOwnCellAttacked(getKingIndex(mover()))) {
            GameOutcome outcome{Termination::Win_Checkmate, opponent(mover())};
            recordOutcome(outcome);
            return outcome;
        } else {
            /// On Stalemate, Glinski chess awards the last mover 3/4 point.
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
                    return outcome;
                } else {
                    break;  // Not insufficient resources
                }
            }
        }
    }

    // ---------- Draw by 3x Board repetition ----------
    if (_currentCounter > 2) {
        if (_zHashToCounters.at(_zHashes.at(_currentCounter - 1)).size() == 3) {
            GameOutcome outcome{Termination::Draw_3xBoardRepetition};
            recordOutcome(outcome);
            return outcome;
        }
    }
    // ---------- Draw by 50 Move rule ----------
    if (_nonProgressCounter >= 50) {
        GameOutcome outcome{Termination::Draw_50MoveRule};
        recordOutcome(outcome);
        return outcome;
    }

    // Cached value for game in progress must differ from that representing an emtpy cache).
    GameOutcome outcome{Termination::None};
    recordOutcome(outcome);
    return outcome;
}

template<>
bool Board<Glinski>::getIsGameOver() cache_const {
    bool hasTermination = getOptOutcome().has_value();
    if (!hasTermination) {
        return false;
    }
    Termination t = getOptOutcome().value().termination();
    return t != Termination::None;
}

/// \todo Return bool or std::optional<Termination>?
template<>
void Board<Glinski>::moveExec(const Move& move) {
    Scope scope{"Board::moveExec"};
    // TODO: Find why these sometimes don't match
    bool verbose = hexchess::general_verbose;
    bool debug{true};

    print(cout, scope(), "Board=", name(), "[1], counter=", currentCounter(),
         ". Making assertion for move=", move.move_pgn_string(false), "\n");
    // Note: A computer player exploring the game tree can make a move capturing a King.
    //       This frees them up from having to continually detect for check.
    // assert(!move.optCaptured().has_value()
    //    || move.optCaptured().value() != PieceType::King);
    print(cout, scope(), "Board=", name(), "[2], counter=", currentCounter(),
         ". Making consistency check for move=", move.move_pgn_string(false), "\n");
    if (debug) {
        _bitsConsistencyTest();
    }

    // ========== Capture ==========
    // The destination is either empty, or it's a capture
// cout << "====================\n";
// print(cout, scope(), "\n",
//     "\tboard=", name(), "\n",
//     "\tmove.from()=", move.from(), "\n",
//     "\tmove.to()=", move.to(), "\n",
//     "\tmove.mover()=", move.mover(), "\n",
//     "\tis any piece at to?: ", isPieceAt(move.to()), "\n",
//     "\tis mover's (", move.mover(), ") piece at to?: ",
//         isPieceAt(move.to(), move.mover()), "\n",
//     "\tis opponent's (", opponent(move.mover()), ") piece at to?: ",
//         isPieceAt(move.to(), opponent(move.mover())), "\n",
//     "\tmove.isCapture()=", move.isCapture(), "\n",
//     "\tboard before assertion:\n");

    assert(!isPieceAt(move.to())
           || (isPieceAt(move.to(), opponent(move.mover()))
               && move.isCapture()
              )
          );
    if (move.optCaptured().has_value()) {
        Index capInd = move.isEnPassant()
                     ? _optEpIndex.value()
                     : move.to();
        Color c = opponent(move.mover());
        PieceType pt = move.optCaptured().value();
        _bitsReset(capInd, c, pt);
    }
    _bitsConsistencyTest();

    // ========== Move ==========
    _bitsMove(_anyPieceBits, move.from(), move.to());
    _bitsMove(_colorToAnyPieceBits, move.mover(), move.from(), move.to());

    switch (move.pieceType()) {
    case PieceType::King:
        _bitsMove(_colorToKingBits, move.mover(), move.from(), move.to());
        setKingIndex(move.to(), move.mover());
        break;
    case PieceType::Queen:
        _bitsMove(_colorToQueenBits, move.mover(), move.from(), move.to());
        break;
    case PieceType::Rook:
        _bitsMove(_colorToRookBits, move.mover(), move.from(), move.to());
        break;
    case PieceType::Bishop:
        _bitsMove(_colorToBishopBits, move.mover(), move.from(), move.to());
        break;
    case PieceType::Knight:
        _bitsMove(_colorToKnightBits, move.mover(), move.from(), move.to());
        break;
    case PieceType::Pawn:
        _bitsMove(_colorToPawnBits, move.mover(), move.from(), move.to());
        if (move.optPromotedTo().has_value()) {
            changePieceType(move.to(), move.mover(), PieceType::Pawn,
                move.optPromotedTo().value());
        }
        break;
    default:
        throw std::logic_error("Board::moveMake: Unrecognized PieceType");
    }
    _bitsConsistencyTest();

    // ---------- Castling ----------
    // TODO: Add support for castling
    // When a Rook is moved from index, and this Bits tests true at index,
    //     then that bit should be cleared.
    // When a King is moved, this Bits value should be cleared.
    // std::map<Color, typename V::Bits> _colorToRookCastlingAvailabilityBits;

    // ---------- Set en passant cell ----------
    if (move.pieceType() == PieceType::Pawn
       && abs(V::row(move.to() - move.from())) == 4)  // 4 half-rows = 2 rows
    {
        // One step forward from Pawn's starting position.
        // Note: Taking the 1st of the advance positions is variant-specific.
        if (verbose) {
            print(cout, scope(), "Board=", name(), "[3], counter=", currentCounter(),
                ". Setting e.p. index\n");
        }
        Index epInd = V::colorToPawnAdvance1Indices
                          .at(move.mover()).at(move.from())[0];
        _optEpIndex = std::make_optional(epInd);
    } else {
        _optEpIndex = std::nullopt;
    }

    // ========== Update progress counter ==========
    if (move.isProgressMove()) {
        _nonProgressCounter = 0;
    } else {
        _nonProgressCounter++;
    }
    _nonProgressCounters.push_back(_currentCounter);

    // ========== Update hash history ==========
    ZHash hash = zobristHash();
    if (_zHashToCounters.contains(hash)) {
        _zHashToCounters[hash] = Shorts{1, _currentCounter};
    } else {
        _zHashToCounters[hash].push_back(_currentCounter);
        if (_zHashToCounters[hash].size() == 3) {
           _cache.optOutcome = std::make_optional<GameOutcome>(
                                   Termination::Draw_3xBoardRepetition);
        }
    }
    _zHashes.push_back(hash);

    // ========== Update move stack ==========
    _moveStack.push_back(move);

    // ========== Next move and isGameOver check ==========
    _cache.clear(_currentCounter);
    _currentCounter++;
    _mover = nextPlayer(move.mover());
    // (void) getLegalMoves(_mover);
    // (void) getOptOutcome();

    // if (getIsGameOver()) {
    //     string msg = getOptOutcome().value().game_outcome_reader_string(move.mover());
    //     cout << "Game is over: " << msg << "\n";
    // }

    // ---------- Clear Update cache ----------
    if (verbose) {
        _bitsConsistencyTest();
        print(cout, scope(), "Board=", name(), "[4], counter=", currentCounter(),
            ". ========== ",
            " Mover ", color_long_string(move.mover()),
            " completed move #", _currentCounter + 1, ": ", move.move_pgn_string(false),
            " ==========\n");
    }
}

template<>
void Board<Glinski>::moveRedo(const Move& move) {
    throw NotImplementedException{"Move::moveRedo"};
}

#include <type_traits>
/// \todo Use Qt's Undo framework
template<>
void Board<Glinski>::moveUndo(const Move& move) {
    Scope scope{"Board::moveUndo"};

    print(cout, scope(), "Counter=", currentCounter(),
        ", moveStack.size()=", _moveStack.size(),
        ", move to undo=", move.move_pgn_string(false),
        ", move.moveEnum()=", move.moveEnum(), ". Entering\n");

    // TODO: Sanity check that there is actually a Move history
    switch (move.moveEnum()) {
    case MoveEnum::Castling:
        // TODO: Support castling for non-Glinski variants.
        break;
    case MoveEnum::EnPassant: {
        // Replace captured Pawn
        // Note: The following is variant-specific
    print(cout, scope(), "Counter=", currentCounter(),
        ", moveStack.size()=", _moveStack.size(),
        ", move to undo=", move.move_pgn_string(false),
        ", Undoing en passant move\n");
        Color opp = opponent(move.mover());
        HexDir oppFwd{V::pawnAdvanceDirs(opp)[0]};
        HexPos captPos{V::indexToPos(move.to()) + oppFwd};
        Index captInd = V::posToIndex(captPos);
        addPiece(captInd, opp, PieceType::Pawn);
        break;
    }
    case MoveEnum::PawnPromotion:
        print(cout, scope(), "Counter=", currentCounter(),
            ", moveStack.size()=", _moveStack.size(),
            ", move to undo=", move.move_pgn_string(false),
            ", PawnPromotion. Changing piece type back to Pawn.\n");
        // Convert promoted piece back to Pawn
        changePieceType(move.to(), move.mover(),
            move.optPromotedTo().value(), PieceType::Pawn);
        print(cout, scope(), "Counter=", currentCounter(),
            ", moveStack.size()=", _moveStack.size(),
            ", move to undo=", move.move_pgn_string(false),
            ", PawnPromotion. Changing piece type back to Pawn.\n");
        break;
    default:
        // Simple move: Handled below
        break;
    }
    print(cout, scope(), "Counter=", currentCounter(),
        ", moveStack.size()=", _moveStack.size(),
        ", move to undo=", move.move_pgn_string(false),
        ", PawnPromotion. Moving piece back to original position.\n");
    // Move piece back to original location
    movePiece(move.to(), move.from(), move.mover(), move.pieceType());

    // Replace captured piece, if any
    if (move.isCapture()) {
        print(cout, scope(), "Counter=", currentCounter(),
            ", moveStack.size()=", _moveStack.size(),
            ", move to undo=", move.move_pgn_string(false),
            ", PawnPromotion. Replacing captured piece.\n");
        addPiece(move.to(), opponent(move.mover()), move.optCaptured().value());
    }

    // \todo Modify the following to support moveRedo

    // Undo Zobrist hash history
    print(cout, scope(), "Counter=", currentCounter(),
        ", moveStack.size()=", _moveStack.size(),
        ", move to undo=", move.move_pgn_string(false),
        ", PawnPromotion. Undoing Zobrist hash history.\n");
    ZHash lastHash = _zHashes.at(_zHashes.size() - 1);
    _zHashes.pop_back();  // Remove from per-ply vector of board hashes
    _zHashToCounters[lastHash].pop_back();  // Remove last occurrence of this hash

    // Undo nonProgressCounters
    print(cout, scope(), "Counter=", currentCounter(),
        ", moveStack.size()=", _moveStack.size(),
        ", move to undo=", move.move_pgn_string(false),
        ", PawnPromotion. Undoing non-progress counters.\n");
    _nonProgressCounters.pop_back();

    // _optEpIndex
    print(cout, scope(), "Counter=", currentCounter(),
        ", moveStack.size()=", _moveStack.size(),
        ", move to undo=", move.move_pgn_string(false),
        ", PawnPromotion. Undoing optEpIndex.\n");
    for (Size k = 0; k < _moveStack.size(); ++k) {
        print(cout, scope(), "Counter=", currentCounter(),
            ", moveStack[", k, "]=", _moveStack.at(k).move_pgn_string(false), "\n");
    }
    const Move& prevMove = _moveStack.at(_currentCounter - 1);
    if (abs(V::row(prevMove.to()) - V::row(prevMove.from())) == 4) {
        _optEpIndex = V::average(prevMove.to(), prevMove.from());
    } else {
        _optEpIndex = std::nullopt;
    }

    print(cout, scope(), "Counter=", currentCounter(),
        ", moveStack.size()=", _moveStack.size(),
        ", move to undo=", move.move_pgn_string(false),
        ", PawnPromotion. Undoing mover and moveStack.\n");
    _mover = prevPlayer(move.mover());
    _moveStack.pop_back();

    // Undo HalfMoveCounter and mover Color
    _currentCounter--;

    // Update cache
    _cache.clear(_currentCounter);
    // (void) getLegalMoves(_mover);
}

// ========================================
// Reading and writing game state

template<>
const Indices Board<Glinski>::attackers(Index tgtInd, const Moves& moves) const {
    Indices result{};

    for (const Move& move : moves) {
        if (move.to() == tgtInd) { result.push_back(move.to()); }
    }
    return result;
}

template<>
bool Board<Glinski>::isCheckmate() cache_const{
    return getIsGameOver()
        && getOptOutcome().value().termination() == Termination::Win_Checkmate;
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
// Indices Board<Glinski>::pinningIndices(Index tgtInd, Color c) const {
//     throw NotImplementedException{"Board<Glinski>::pinningIndides"};
// }

}  // namespace hexchess::core

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

#include <string>
#include <map>

#include "util_hexchess.h"
#include "styleicon.h"

using hexchess::core::Color;
using hexchess::core::PieceType;


IconPaths iconSettings {
    { Color::Black,
        {
        { PieceType::King,   "./images/pieces/bw_bgt/Chess_kdt45.svg" },
        { PieceType::Queen,  "./images/pieces/bw_bgt/Chess_qdt45.svg" },
        { PieceType::Rook,   "./images/pieces/bw_bgt/Chess_rdt45.svg" },
        { PieceType::Bishop, "./images/pieces/bw_bgt/Chess_bdt45.svg" },
        { PieceType::Knight, "./images/pieces/bw_bgt/Chess_ndt45.svg" },
        { PieceType::Pawn,   "./images/pieces/bw_bgt/Chess_pdt45.svg" }
        }
    },
    { Color::White,
        {
        { PieceType::King,   "./images/pieces/bw_bgt/Chess_klt45.svg" },
        { PieceType::Queen,  "./images/pieces/bw_bgt/Chess_qlt45.svg" },
        { PieceType::Rook,   "./images/pieces/bw_bgt/Chess_rlt45.svg" },
        { PieceType::Bishop, "./images/pieces/bw_bgt/Chess_blt45.svg" },
        { PieceType::Knight, "./images/pieces/bw_bgt/Chess_nlt45.svg" },
        { PieceType::Pawn,   "./images/pieces/bw_bgt/Chess_plt45.svg" }
        }
    }
};

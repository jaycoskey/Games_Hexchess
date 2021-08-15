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

#include <sys/types.h>

#include <optional>
#include <type_traits>

#include "move.h"

#include "util_hexchess.h"


using CellStatus = ushort;
using Real = float;


extern std::vector<Real> cos_at;
extern std::vector<Real> sin_at;

constexpr CellStatus CellStatus_None      = 0;
constexpr CellStatus CellStatus_LastMoved = 1 << 0;
constexpr CellStatus CellStatus_Selected  = 1 << 1;
constexpr CellStatus CellStatus_Warning   = 1 << 2;

bool isCellLastMoved(CellStatus status);
bool isCellSelected(CellStatus status);
bool isCellWarning(CellStatus status);

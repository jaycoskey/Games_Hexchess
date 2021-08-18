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

#include <cmath>

#include "util.h"  // For pi
#include "util_ui.h"  // For Real


using std::vector;

using hexchess::core::pi;


vector<Real> cos_at{ []() {
    vector<Real> result{};

    for (int k = 0; k <= 6; ++k) {
        result.push_back(std::cos(k * 2 * pi / 6));
    }
    return result;
}()};

vector<Real> sin_at { []() {
    vector<Real> result{};

    for (int k = 0; k <= 6; ++k) {
        result.push_back(std::sin(k * 2 * pi / 6));
    }
    return result;
}()};

bool isCellLastMoved(CellStatus status) {
    return (status & CellStatus_LastMoved) != CellStatus_None;
}

bool isCellSelected(CellStatus status) {
    return (status & CellStatus_Selected)  != CellStatus_None;
}

bool isCellWarning(CellStatus status) {
    return (status & CellStatus_LastMoved) != CellStatus_None;
}

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

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <math.h>


namespace hexchess::core {

// ========================================
// Classes

/// \brief A shorthand version of <utility>'s std::make_pair.
template <class T1, class T2>
std::pair<T1,T2> mkPair(T1 x, T2 y) { return std::make_pair(x, y); }

/// \brief Thrown by any class that is defined, but not implemented.
class NotImplementedException : public std::exception {
};

// ========================================
// Methods

/// \brief A string reversal function, used to print bitsets.
inline std::string reved(const std::string& s) {
    std::string result{s};
    std::reverse(result.begin(), result.end());
    return result;
}

/// \brief A handy transform function that appends to an output vector.
template <class InputIt, typename T, class UnaryPred, class UnaryOp>
void transform_push_back_if(InputIt src, InputIt last,
    std::vector<T>& dest, UnaryPred pred, UnaryOp xform
    )
{
    for ( ; src != last; ++src) {
        if (pred(*src)) {
            dest.push_back(xform(*src));
        }
    }
}

// ========================================
// Values
/// \todo When moving to C++20, define pi as std::numbers::pi from <numbers>.

static constexpr double pi = M_PI;

}  // namespace hexchess::core

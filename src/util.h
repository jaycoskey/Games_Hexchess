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
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <math.h>


namespace hexchess {

// ========================================
// Classes

/// \brief Thrown by any class that is defined, but not implemented.
class NotImplementedException : public std::logic_error {
public:
    NotImplementedException(const char* msg="Not yet implemented")
        : std::logic_error{msg}
    { };
};

// ========================================
// Methods

/// \brief Return random element from vector
///
/// If needed, templatize further w/ collections: enable_if<has_const_iterator>...
template <typename T>
inline const T& chooseRandom(const std::vector<T>& collection) {
    static std::random_device dev{"/dev/random"};
    static std::mt19937 prng(dev());

    std::uniform_int_distribution<> randomChoice(0, collection.size());
    return collection[randomChoice(prng)];
}

template <class T, class V>
const std::vector<T> maxValues(const std::vector<T>& ts, std::function<V(const T&)> f) {
    V bestValue = f(ts[0]);
    // Pass #1
    for (const T& t : ts) {
        V value = f(t);
        if (value > bestValue) {
            bestValue = value;
        }
    }

    // Pass #2
    std::vector<T> result{};
    std::copy_if(ts.begin(), ts.end(), result.begin(),
                 [&](const T& t) { return f(t) == bestValue; }
                );
    return result;
}

/// \brief A shorthand version of <utility>'s std::make_pair.
template <class T1, class T2>
std::pair<T1,T2> mkPair(T1 x, T2 y) { return std::make_pair(x, y); }

/// \brief A string reversal function, used to print bitsets.
inline std::string reved(const std::string& s) {
    std::string result{s};
    std::reverse(result.begin(), result.end());
    return result;
}

inline std::vector<std::string> split(char delim, const std::string& input) {
    std::vector<std::string> result{};

    std::size_t begin;
    std::size_t end = 0;
    while ((begin = input.find_first_not_of(delim, end)) != std::string::npos) {
        end = input.find(delim, begin);
        result.push_back(input.substr(begin, end - begin));
    }
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

}  // namespace hexchess

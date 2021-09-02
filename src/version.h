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

#include <sstream>
#include <string>


namespace hexchess {

constexpr int version_major = 0;
constexpr int version_minor = 0;
constexpr int version_patch = 1;

std::string version_string() {
    static std::string version_string{ []()
        {
            std::ostringstream oss;
            oss << version_major << '.'
                << version_minor << '.'
                << version_patch;
            return oss.str();
        }()
    };
    return version_string;
}

}  // namespace hexchess

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

#include <map>
#include <variant>

#include "stylemeasure.h"


std::variant<int, Real> measureSetting(MeasureEnum mEnum) {
    static const MeasureSettings measureSettings{
        { MeasureEnum::Board_BorderWidth, int{0},       },
        { MeasureEnum::Board_Margin,      int{10},      },

        // For flat-top hexagons: 2 / sqrt(3) = 1.1547
        { MeasureEnum::Cell_AspectRatio,  Real{1.1547}, },
        { MeasureEnum::Cell_BorderWidth,  int{0}        },
        { MeasureEnum::Cell_Height,       int{60}       },
        { MeasureEnum::Cell_Margin,       int{0}        }
    };
    return measureSettings.at(mEnum);
}

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

#include <map>

#include <QColor>

#include "util_hexchess.h"
#include "variant.h"
#include "stylecolor.h"

using hexchess::core::Glinski;
using hexchess::core::Index;


ColorSettings colorSettings {
    { ColorEnum::Board_Background,          {"white"}   },

    { ColorEnum::Cell_Border,               {"black"}       },
    { ColorEnum::Cell_Margin,               {"black"}       },
    { ColorEnum::Cell_Status_LastMoved,     {"blue"}        },
    { ColorEnum::Cell_Status_Selected,      {"green"}       },
    { ColorEnum::Cell_Status_Warning,       {255, 192, 203} },
    { ColorEnum::Cell_Shade_Dark,           {222, 188, 153} },
    { ColorEnum::Cell_Shade_Light,          {239, 241, 219} },
    { ColorEnum::Cell_Shade_Medium,         {220, 208, 186} },
    { ColorEnum::Cell_Text,                 {"black"}       },
    { ColorEnum::Cell_Text_Status_Selected, {255, 63, 255}  },
};

QColor cellQColor(Index index)
{
    ColorEnum shade = cellShades[(Glinski::hex0(index) + Glinski::hex1(index)) % 3];
    return colorSettings[shade];
}

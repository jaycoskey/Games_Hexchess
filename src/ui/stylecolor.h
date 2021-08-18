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

#include <map>

#include <QColor>

#include "util_hexchess.h"


using hexchess::core::Index;


enum class ColorEnum;
using ColorSettings = std::map<ColorEnum, QColor>;


enum class ColorEnum {
    Board_Background,

    Cell_Border,
    Cell_Margin,

    // TODO: Determine which object is painted to reflect status
    Cell_Status_LastMoved,
    Cell_Status_Selected,
    Cell_Status_Warning,
    Cell_Shade_Dark,
    Cell_Shade_Light,
    Cell_Shade_Medium,
    Cell_Text,
    Cell_Text_Status_Selected
};
extern ColorSettings colorSettings;

const ColorEnum cellShades[3] {
    ColorEnum::Cell_Shade_Dark,
    ColorEnum::Cell_Shade_Medium,
    ColorEnum::Cell_Shade_Light
    };

QColor cellQColor(Index index);

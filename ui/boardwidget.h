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

#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <QColor>
#include <QSvgWidget>
#include <QtWidgets>

#include "util_hexchess.h"
#include "variant.h"
#include "bitboard.h"

#include "stylecolor.h"
#include "stylemeasure.h"


using hexchess::core::Bitboard;
using hexchess::core::Glinski;
using hexchess::core::Index;
using hexchess::core::OptPieceType;


/// Glinski hexagonal chessboard
class BoardWidget : public QWidget
{
    Q_OBJECT
public:
    BoardWidget(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    // TODO: Handle style settings in Qt fashion.
    class Cell {
    public:
        Cell(Index index,
            OptPieceType optPieceType=std::nullopt,
            CellStatus status=CellStatus_None
            );

        void clearLastMoved() { status &= ~CellStatus_LastMoved; }
        void clearSelected()  { status &= ~CellStatus_Selected; }
        void clearWarning()   { status &= ~CellStatus_Warning; }

        bool isLastMoved() const { return isCellLastMoved(status); }
        bool isSelected()  const { return isCellSelected(status); }
        bool isWarning()   const { return isCellWarning(status); }

        void setLastMoved() { status |= CellStatus_LastMoved; }
        void setSelected()  { status |= CellStatus_Selected; }
        void setWarning()   { status |= CellStatus_Warning; }

        bool isEmpty() { return optPieceType == std::nullopt; }

        Index index;
        QPolygon polygon;

        OptPieceType optPieceType;
        std::string text;
        CellStatus status;

    private:
        const std::pair<Real, Real> _cellCenterCoords(
            Index index,
            Real boardMargin, Real cellBorder, Real cellHeight, Real cellMargin, Real cellWidth
            );
    };

    std::vector<Cell> cells;
    Bitboard<Glinski> bitboard{};
};

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

#include "board.h"
#include "fen.h"
#include "move.h"
#include "util_hexchess.h"
#include "variant.h"

#include "stylecolor.h"
#include "stylemeasure.h"

using hexchess::core::Board;
using hexchess::core::Fen;
using hexchess::core::Glinski;
using hexchess::core::Index;
using hexchess::core::Move;
using hexchess::core::OptColorPieceType;


/// Glinski hexagonal chessboard
class BoardWidget : public QWidget
{
    Q_OBJECT
public:
    BoardWidget(QWidget *parent = nullptr);
    void initializeBoard(const Fen<Glinski>& fen);
    void execMove(const Move& move);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    static const int  _boardBorder;
    static const int  _boardMargin;
    static const Real _cellAspectRatio;
    static const int  _cellBorder;
    static const int  _cellHeight;
    static const int  _cellMargin;

    static const std::pair<int, int> _cellCenterCoords(Index index);

    // TODO: Handle style settings in Qt fashion.
    class Cell {
    public:
        Cell(Index index, CellStatusFlags status=CellStatus_None);

        void clearLastMoved() { status &= ~CellStatus_LastMoved; }
        void clearSelected()  { status &= ~CellStatus_Selected; }
        void clearWarning()   { status &= ~CellStatus_Warning; }

        bool isLastMoved() const { return isCellLastMoved(status); }
        bool isSelected()  const { return isCellSelected(status); }
        bool isWarning()   const { return isCellWarning(status); }

        void setLastMoved() { status |= CellStatus_LastMoved; }
        void setSelected()  { status |= CellStatus_Selected; }
        void setWarning()   { status |= CellStatus_Warning; }

        Index index;
        QPolygon polygon;
        std::string text;
        CellStatusFlags status;
    };

    Board<Glinski> _board;
    std::vector<Cell> _cells;
};

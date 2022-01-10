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

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <variant>

#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <QtWidgets>

#include "board.h"
#include "fen.h"
#include "move.h"
#include "util_hexchess.h"
#include "variant.h"

#include "boardwidget.h"
#include "stylecolor.h"
#include "stylefont.h"
#include "styleicon.h"
#include "stylemeasure.h"
#include "util_ui.h"

using std::cout;
using std::string;

using std::map;
using std::pair;

using hexchess::core::Board;
using hexchess::core::Color;
using hexchess::core::Fen;
using hexchess::core::Glinski;
using hexchess::core::Index;
using hexchess::core::Move;
using hexchess::core::PieceType;


const int BoardWidget::_boardBorder{ []()
    {
        return std::get<int>(measureSetting(MeasureEnum::Board_BorderWidth));
    }()
};
const int BoardWidget::_boardMargin{ []()
    {
        return std::get<int>(measureSetting(MeasureEnum::Board_Margin));
    }()
};
const Real BoardWidget::_cellAspectRatio{ []()
    {
        return std::get<Real>(measureSetting(MeasureEnum::Cell_AspectRatio));
    }()
};
const int BoardWidget::_cellBorder{ []()
    {
        return std::get<int>(measureSetting(MeasureEnum::Cell_BorderWidth));
    }()
};
const int BoardWidget::_cellHeight{ []()
    {
        return std::get<int>(measureSetting(MeasureEnum::Cell_Height));
    }()
};
const int BoardWidget::_cellMargin{ []()
    {
        return std::get<int>(measureSetting(MeasureEnum::Cell_Margin));
    }()
};

// =======================================

/// Returns screen coords of center of cell with given index
const pair<int, int> BoardWidget::_cellCenterCoords(Index index) {
    assert(index >= 0 && index < Glinski::CELL_COUNT);

    static map<Index, pair<int, int>> resultCache{};
    if (resultCache.contains(index)) {
        return resultCache.at(index);
    }

    Real cellWidth = _cellAspectRatio * _cellHeight;
    Real w = 0.5 * cellWidth;
    Real cellFlatWidth = w;

    int hex0 = Glinski::hex0(index);
    int hex1 = Glinski::hex1(index);

    Real x_leftMostCenter = _boardMargin + _cellMargin + w;  // x of center A1 .. A6
    Real x_perFileOffset = 0.5 * (cellWidth + cellFlatWidth) + 2 * _cellMargin;
    Real x = x_leftMostCenter + hex0 * x_perFileOffset;

    Real y_centerOfA1 = _boardMargin + 8 * _cellHeight + 17 * _cellMargin;  // y of center of A1
    Real y_offsetFromA1 = (_cellHeight + 2 * _cellMargin) * (0.5 * hex0 - hex1);
    Real y = y_centerOfA1 + y_offsetFromA1;

    pair<int, int> result{x, y};
    resultCache[index] = result;
    return result;
}

BoardWidget::BoardWidget(QWidget *parent)
    : QWidget{parent},
      _board{"BoardWidget", false}
{
    setWindowTitle(tr("Hexchess"));

    // Initialize cells. Board layout is initialized by initializeBoard
    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        _cells.push_back(Cell(index, CellStatus_None));
    }
    Real cellWidth = _cellAspectRatio * _cellHeight;

    int widgetWidth = 2 * _boardMargin + 8.5 * cellWidth + 22 * _cellMargin;
    int widgetHeight = 2 * _boardMargin + 11 * _cellHeight + 22 * _cellMargin;
    resize(widgetWidth, widgetHeight);
    setMinimumWidth(widgetWidth);
    setMinimumHeight(widgetHeight);
}

void BoardWidget::initializeBoard(const Fen<Glinski>& fen) {
    _board.initialize(fen);
}

void BoardWidget::execMove(const Move& move) {
    cout << "BoardWidget:execMove calling moveExec with move=" << move.move_pgn_string(false) << "\n";
    _board.moveExec(move);
    repaint();
}

void BoardWidget::keyPressEvent(QKeyEvent *event)
{
    (void) event;  // Prevent compiler warning
}

void BoardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    auto pCell = std::find_if(_cells.begin(), _cells.end(),
        [point](const Cell& cell) {
            return cell.polygon.containsPoint(point, Qt::OddEvenFill);
        }
    );
    if (pCell != _cells.end() && !pCell->isSelected()) {
        pCell->setSelected();
        update(pCell->polygon.boundingRect());
    }
}

// TODO: Paint cell margins
void BoardWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter{this};
    painter.setRenderHint(QPainter::Antialiasing);
    if (event) {
        painter.fillRect(event->rect(), colorSettings[ColorEnum::Board_Background]);
    }

    QFont qFont{QString::fromStdString(board_font)};
    qFont.setPixelSize(board_font_size);
    painter.setFont(qFont);

    const QColor boardBorderColor{colorSettings[ColorEnum::Board_Background]};
    const QColor selectedCellColor{colorSettings[ColorEnum::Cell_Status_Selected]};
    const QColor selectedTextColor{colorSettings[ColorEnum::Cell_Text_Status_Selected]};
    const QColor textColor{colorSettings[ColorEnum::Cell_Text]};

    // Paint board
    for (Cell& cell : _cells)
    {
        QPainterPath path;
        path.addPolygon(cell.polygon);

        // Border
        QPen pen{boardBorderColor};
        pen.setWidth(_boardBorder);
        painter.setPen(pen);
        QColor brushColor = isCellSelected(cell.status)
                                ? selectedCellColor
                                : cellQColor(cell.index);
        painter.setBrush(brushColor);
        painter.drawPath(path);

        // Text
        if (_board.isEmpty(cell.index)) {
            painter.setPen(isCellSelected(cell.status)
                ? selectedTextColor
                : textColor);
            painter.drawText(
                cell.polygon.boundingRect(),
                Qt::AlignCenter,
                QString::fromStdString(cell.text)
                );
        }
    }

    // Paint pieces
    for (Cell& cell : _cells) {
        Index index = cell.index;
        if (!_board.isEmpty(cell.index)) {
            Color c = _board.getColorAt(index);
            PieceType pt = _board.getPieceTypeAt(index, c);
            string iconPath = iconSettings[c][pt];
            QString qIconPath = QString::fromStdString(iconPath);
            QSvgRenderer renderer{qIconPath /* , parent=this */ };

            QPainter painter{this};
            /// \todo Use a slightly smaller QRect
            QRect qRect = cell.polygon.boundingRect();
            painter.setViewport(qRect);
            renderer.render(&painter);
        }
    }
}

// ========================================
// BoardWidget::Cell

BoardWidget::Cell::Cell(Index index, CellStatusFlags status)
    : index{index},
      polygon{7},
      text{Glinski::cellName(index)},
      status{status}
{
    assert(index >= 0 && index < Glinski::CELL_COUNT);

    QColor boardColor_bg = colorSettings[ColorEnum::Board_Background];

    QColor cellColor_border = colorSettings[ColorEnum::Cell_Border];
    QColor cellColor_margin = colorSettings[ColorEnum::Cell_Margin];

    QColor cellColor_lastMovedStatus = colorSettings[ColorEnum::Cell_Status_LastMoved];
    QColor cellColor_selectedStatus = colorSettings[ColorEnum::Cell_Status_Selected];
    QColor cellColor_warningStatus = colorSettings[ColorEnum::Cell_Status_Warning];

    const auto [cx, cy] = _cellCenterCoords(index);
    // QPolygon polygon{7};
    Real h = 0.5 * _cellHeight;
    Real cellWidth = _cellAspectRatio * _cellHeight;
    Real w = 0.5 * cellWidth;
    Real vscale = 2 / sqrt(3);
    for (int edgeNum = 0; edgeNum <= 6; ++edgeNum) {
        polygon.setPoint(edgeNum, cx + w * cos_at[edgeNum], cy + vscale * h * sin_at[edgeNum]);
    }
}

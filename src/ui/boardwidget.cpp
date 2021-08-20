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

#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>

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

using std::pair;

using hexchess::core::Color;
using hexchess::core::Index;
using hexchess::core::Glinski;
using hexchess::core::PieceType;


BoardWidget::BoardWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Hexchess"));

    // Initialize Board
    for (Index index = 0; index < Glinski::CELL_COUNT; ++index) {
        cells.push_back(Cell(index, std::nullopt, CellStatus_None));
    }
    Real boardMargin = measureSettings[MeasureEnum::Board_Margin];
    Real cellHeight  = measureSettings[MeasureEnum::Cell_Height];
    Real cellMargin  = measureSettings[MeasureEnum::Cell_Margin];

    Real cellAspectRatio = measureSettings[MeasureEnum::Cell_AspectRatio];
    Real cellWidth   = cellAspectRatio * cellHeight;

    int widgetWidth = 2 * boardMargin + 8.5 * cellWidth + 22 * cellMargin;
    int widgetHeight = 2 * boardMargin + 11 * cellHeight + 22 * cellMargin;
    resize(widgetWidth, widgetHeight);
    setMinimumWidth(widgetWidth);
    setMinimumHeight(widgetHeight);
}

void BoardWidget::keyPressEvent(QKeyEvent *event)
{
    (void) event;  // Prevent compiler warning
}

void BoardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    auto pCell = std::find_if(cells.begin(), cells.end(),
        [point](const Cell& c) {
            return c.polygon.containsPoint(point, Qt::OddEvenFill);
        }
    );
    if (pCell != cells.end() && !pCell->isSelected()) {
        pCell->setSelected();
        update(pCell->polygon.boundingRect());
    }
}

// TODO: Paint cell margins
void BoardWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter{this};
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(event->rect(), colorSettings[ColorEnum::Board_Background]);

    QFont qFont{QString::fromStdString(board_font)};
    qFont.setPixelSize(board_font_size);
    painter.setFont(qFont);

    const QColor boardBorderColor{colorSettings[ColorEnum::Board_Background]};
    const QColor selectedCellColor{colorSettings[ColorEnum::Cell_Status_Selected]};
    const QColor selectedTextColor{colorSettings[ColorEnum::Cell_Text_Status_Selected]};
    const QColor textColor{colorSettings[ColorEnum::Cell_Text]};

    for (Cell& cell : cells)
    {
        QPainterPath path;
        path.addPolygon(cell.polygon);

        // Color
        QPen pen(boardBorderColor, measureSettings[MeasureEnum::Board_BorderWidth]);
        painter.setPen(pen);
        QColor brushColor = isCellSelected(cell.status)
                                ? selectedCellColor
                                : cellQColor(cell.index);
        painter.setBrush(brushColor);
        painter.drawPath(path);

        // SVG
        Index index = cell.index;
        if (!board.isEmpty(cell.index)) {
            /// \todo Get piece SVGs to render inside Cells
            Color c = board.getColorAt(index);
            PieceType pt = board.getPieceTypeAt(index);
            string iconPath = iconSettings[c][pt];
            QString qIconPath = QString::fromStdString(iconPath);
            QSvgRenderer renderer(qIconPath);
            QImage svgImg(60, 60, QImage::Format_ARGB32);
            svgImg.fill(0xddbbaa99);
            QPainter painter(&svgImg);
            renderer.render(&painter);

            // QDirIterator it(".");
            // cout << "Contents of the current directory:\n";
            // while (it.hasNext()) {
            //     QFile f(it.next());
            //     cout << "\t" << f.fileName().toStdString() << "\n";
            // }
            // cell.pSvg = new QSvgWidget(qPath, &cell.polygon);
            // cell.pSvg->svgWidget = QtSvg.QScgWidget();
        }

        // Text
        if (!cell.isEmpty()) {
            painter.setPen(isCellSelected(cell.status) ? selectedTextColor : textColor);
            painter.drawText(
                cell.polygon.boundingRect(),
                Qt::AlignCenter,
                QString::fromStdString(cell.text)
                );
        }
    }
}

BoardWidget::Cell::Cell(
    Index index,
    OptPieceType optPieceType,
    CellStatus status
    )
    :
    index{index},
    polygon{7},
    optPieceType{optPieceType},
    text{Glinski::cellName(index)},
    status{status}
{
    assert(index >= 0 && index < Glinski::CELL_COUNT);

    Real boardMargin = measureSettings[MeasureEnum::Board_Margin];

    Real cellAspectRatio = measureSettings[MeasureEnum::Cell_AspectRatio];
    Real cellBorder  = measureSettings[MeasureEnum::Cell_BorderWidth];
    Real cellHeight  = measureSettings[MeasureEnum::Cell_Height];
    Real cellMargin  = measureSettings[MeasureEnum::Cell_Margin];
    Real cellWidth   = cellAspectRatio * cellHeight;

    QColor boardColor_bg = colorSettings[ColorEnum::Board_Background];

    QColor cellColor_border = colorSettings[ColorEnum::Cell_Border];
    QColor cellColor_margin = colorSettings[ColorEnum::Cell_Margin];

    QColor cellColor_lastMovedStatus = colorSettings[ColorEnum::Cell_Status_LastMoved];
    QColor cellColor_selectedStatus = colorSettings[ColorEnum::Cell_Status_Selected];
    QColor cellColor_warningStatus = colorSettings[ColorEnum::Cell_Status_Warning];

    const auto [cx, cy] = _cellCenterCoords(index,
                              boardMargin, cellBorder, cellHeight, cellMargin, cellWidth
                              );
    // QPolygon polygon{7};
    Real h = 0.5 * cellHeight;
    Real w = 0.5 * cellWidth;
    Real vscale = 2 / sqrt(3);
    for (int edgeNum = 0; edgeNum <= 6; ++edgeNum) {
        polygon.setPoint(edgeNum, cx + w * cos_at[edgeNum], cy + vscale * h * sin_at[edgeNum]);
    }
}

/// Returns screen coords of center of cell with given index
const pair<Real, Real> BoardWidget::Cell::_cellCenterCoords
    (
    Index index,
    Real boardMargin,
    Real cellBorder,
    Real cellHeight,
    Real cellMargin,
    Real cellWidth
    )
{
    assert(index >= 0 && index < Glinski::CELL_COUNT);
    (void) cellBorder;  // Disable compiler warning? TODO: Add to cellMargin?
    Real w = 0.5 * cellWidth;
    Real cellFlatWidth = w;

    int hex0 = Glinski::hex0(index);
    int hex1 = Glinski::hex1(index);

    Real x_leftMostCenter = boardMargin + cellMargin + w;  // x of center A1 .. A6
    Real x_perFileOffset = 0.5 * (cellWidth + cellFlatWidth) + 2 * cellMargin;
    Real x = x_leftMostCenter + hex0 * x_perFileOffset;

    Real y_centerOfA1 = boardMargin + 8 * cellHeight + 17 * cellMargin;  // y of center of A1
    Real y_offsetFromA1 = (/* 0.85 * */ cellHeight + 2 * cellMargin) * (0.5 * hex0 - hex1);
    Real y = y_centerOfA1 + y_offsetFromA1;

    return pair<Real, Real>{x, y};
}

#include "OthelloCellButton.h"

#include <QPainter>

OthelloCellButton::OthelloCellButton(QWidget *parent)
    : QPushButton(parent),
      cell(OthelloGame::Empty)
{
    setFixedSize(64, 64);
    setFlat(true);
}

void OthelloCellButton::setCell(OthelloGame::Cell newCell)
{
    cell = newCell;
    update();
}

void OthelloCellButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), QColor(0, 140, 60));

    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));

    if (cell == OthelloGame::Empty) {
        return;
    }

    int margin = 4;
    QRect stoneRect = rect().adjusted(margin, margin, -margin, -margin);

    if (cell == OthelloGame::Black) {
        painter.setBrush(Qt::black);
        painter.setPen(Qt::black);
    } else if (cell == OthelloGame::White) {
        painter.setBrush(Qt::white);
        painter.setPen(Qt::black);
    }

    painter.drawEllipse(stoneRect);
}
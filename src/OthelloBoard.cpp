#include "OthelloBoard.h"

#include <QPainter>
#include <QMouseEvent>
#include <algorithm>

OthelloBoard::OthelloBoard(QWidget *parent)
    : QWidget(parent),
      game(nullptr)
{
    setFixedSize(512, 512);
}

void OthelloBoard::setGame(OthelloGame *newGame)
{
    game = newGame;
    update();
}

int OthelloBoard::boardSize() const
{
    return std::min(width(), height());
}

int OthelloBoard::cellSize() const
{
    return boardSize() / OthelloGame::SIZE;
}

int OthelloBoard::offsetX() const
{
    return (width() - boardSize()) / 2;
}

int OthelloBoard::offsetY() const
{
    return (height() - boardSize()) / 2;
}

void OthelloBoard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int size = boardSize();
    int cell = cellSize();
    int x0 = offsetX();
    int y0 = offsetY();

    QRect boardRect(x0, y0, cell * OthelloGame::SIZE, cell * OthelloGame::SIZE);

    painter.fillRect(boardRect, QColor(0, 140, 60));

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(Qt::black, 1));

    for (int i = 0; i <= OthelloGame::SIZE; ++i) {
        int x = x0 + i * cell;
        int y = y0 + i * cell;

        painter.drawLine(x, y0, x, y0 + cell * OthelloGame::SIZE);
        painter.drawLine(x0, y, x0 + cell * OthelloGame::SIZE, y);
    }

    if (!game) {
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing, true);

    int margin = 4;

    for (int row = 0; row < OthelloGame::SIZE; ++row) {
        for (int col = 0; col < OthelloGame::SIZE; ++col) {
            OthelloGame::Cell stone = game->getCell(row, col);

            if (stone == OthelloGame::Empty) {
                continue;
            }

            QRect stoneRect(
                x0 + col * cell + margin,
                y0 + row * cell + margin,
                cell - margin * 2,
                cell - margin * 2
            );

            if (stone == OthelloGame::Black) {
                painter.setBrush(Qt::black);
                painter.setPen(Qt::black);
            } else {
                painter.setBrush(Qt::white);
                painter.setPen(Qt::black);
            }

            painter.drawEllipse(stoneRect);
        }
    }
}

void OthelloBoard::mousePressEvent(QMouseEvent *event)
{
    int cell = cellSize();
    int x = event->pos().x() - offsetX();
    int y = event->pos().y() - offsetY();

    if (x < 0 || y < 0) {
        return;
    }

    int col = x / cell;
    int row = y / cell;

    if (row < 0 || row >= OthelloGame::SIZE ||
        col < 0 || col >= OthelloGame::SIZE) {
        return;
    }

    emit cellClicked(row, col);
}
#ifndef OTHELLOBOARD_H
#define OTHELLOBOARD_H

#include <QWidget>

#include "OthelloGame.h"

class OthelloBoard : public QWidget
{
    Q_OBJECT

public:
    explicit OthelloBoard(QWidget *parent = nullptr);

    void setGame(OthelloGame *game);

signals:
    void cellClicked(int row, int col);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    OthelloGame *game;

    int boardSize() const;
    int cellSize() const;
    int offsetX() const;
    int offsetY() const;
};

#endif
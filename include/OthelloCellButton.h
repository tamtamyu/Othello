#ifndef OTHELLOCELLBUTTON_H
#define OTHELLOCELLBUTTON_H

#include <QPushButton>
#include "OthelloGame.h"

class OthelloCellButton : public QPushButton
{
    Q_OBJECT

public:
    explicit OthelloCellButton(QWidget *parent = nullptr);

    void setCell(OthelloGame::Cell cell);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    OthelloGame::Cell cell;
};

#endif
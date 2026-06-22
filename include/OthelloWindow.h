#ifndef OTHELLOWINDOW_H
#define OTHELLOWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>

#include "OthelloGame.h"
#include "OthelloCellButton.h"

class OthelloWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OthelloWindow(QWidget *parent = nullptr);

private slots:
    void handleCellClicked();
    void resetGame();

private:
    OthelloGame game;

    QWidget *centralWidget;
    QGridLayout *boardLayout;

    QLabel *turnLabel;
    QLabel *scoreLabel;
    QLabel *mistakeLabel;

    QPushButton *resetButton;
    OthelloCellButton *buttons[OthelloGame::SIZE][OthelloGame::SIZE];

    int blackMistakeCount;
    int whiteMistakeCount;

    void setupUI();
    void updateBoard();
    void updateStatus();

    void addMistake(OthelloGame::Cell player);

    void showPassMessage();
    void showFinishMessage();

    QString playerName(OthelloGame::Cell player) const;
};

#endif
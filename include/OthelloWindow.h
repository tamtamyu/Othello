#ifndef OTHELLOWINDOW_H
#define OTHELLOWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

#include "OthelloGame.h"
#include "OthelloBoard.h"

class OthelloWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OthelloWindow(QWidget *parent = nullptr);

private slots:
    void handleCellClicked(int row, int col);
    void resetGame();
    void handleModeChanged(int index);

private:
    enum PlayMode {
        HumanVsHuman,
        HumanVsCpu
    };

    OthelloGame game;
    PlayMode playMode;

    QWidget *centralWidget;
    OthelloBoard *boardWidget;

    QLabel *turnLabel;
    QLabel *scoreLabel;
    QLabel *mistakeLabel;

    QComboBox *modeComboBox;
    QPushButton *resetButton;

    int blackMistakeCount;
    int whiteMistakeCount;

    void setupUI();
    void updateBoard();
    void updateStatus();

    void addMistake(OthelloGame::Cell player);

    bool isCpuTurn() const;
    void runCpuTurn();

    void handleAfterMove();

    void showPassMessage();
    void showFinishMessage();

    QString playerName(OthelloGame::Cell player) const;
};

#endif
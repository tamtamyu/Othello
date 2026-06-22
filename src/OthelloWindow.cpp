#include "OthelloWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

OthelloWindow::OthelloWindow(QWidget *parent)
    : QMainWindow(parent),
      blackMistakeCount(0),
      whiteMistakeCount(0)
{
    setupUI();

    game.reset();
    updateBoard();
    updateStatus();
}

void OthelloWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    QHBoxLayout *topLayout = new QHBoxLayout();

    turnLabel = new QLabel(this);
    scoreLabel = new QLabel(this);
    mistakeLabel = new QLabel(this);
    resetButton = new QPushButton("リセット", this);

    topLayout->addWidget(turnLabel);
    topLayout->addStretch();
    topLayout->addWidget(scoreLabel);
    topLayout->addWidget(mistakeLabel);
    topLayout->addWidget(resetButton);

    boardLayout = new QGridLayout();
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    for (int row = 0; row < OthelloGame::SIZE; ++row) {
        for (int col = 0; col < OthelloGame::SIZE; ++col) {
            buttons[row][col] = new OthelloCellButton(this);

            buttons[row][col]->setProperty("row", row);
            buttons[row][col]->setProperty("col", col);

            connect(buttons[row][col], &QPushButton::clicked,
                    this, &OthelloWindow::handleCellClicked);

            boardLayout->addWidget(buttons[row][col], row, col);
        }
    }

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(boardLayout);

    connect(resetButton, &QPushButton::clicked,
            this, &OthelloWindow::resetGame);

    setWindowTitle("オセロ - 人対人");
    setFixedSize(536, 590);
}

void OthelloWindow::handleCellClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button) return;

    int row = button->property("row").toInt();
    int col = button->property("col").toInt();

    OthelloGame::Cell player = game.getCurrentPlayer();

    if (!game.placeStone(row, col)) {
        addMistake(player);
        updateStatus();
        return;
    }

    OthelloGame::GameState state = game.checkGameState();

    updateBoard();
    updateStatus();

    if (state == OthelloGame::Passed) {
        showPassMessage();
        updateBoard();
        updateStatus();
    } else if (state == OthelloGame::Finished) {
        showFinishMessage();
    }
}

void OthelloWindow::resetGame()
{
    game.reset();

    blackMistakeCount = 0;
    whiteMistakeCount = 0;

    updateBoard();
    updateStatus();
}

void OthelloWindow::addMistake(OthelloGame::Cell player)
{
    if (player == OthelloGame::Black) {
        ++blackMistakeCount;
    } else if (player == OthelloGame::White) {
        ++whiteMistakeCount;
    }
}

void OthelloWindow::updateBoard()
{
    for (int row = 0; row < OthelloGame::SIZE; ++row) {
        for (int col = 0; col < OthelloGame::SIZE; ++col) {
            buttons[row][col]->setCell(game.getCell(row, col));
        }
    }
}

void OthelloWindow::updateStatus()
{
    turnLabel->setText(
        "手番: " + playerName(game.getCurrentPlayer())
    );

    scoreLabel->setText(
        QString("黒: %1  白: %2")
            .arg(game.countStones(OthelloGame::Black))
            .arg(game.countStones(OthelloGame::White))
    );

    mistakeLabel->setText(
        QString("お手付き 黒: %1回  白: %2回")
            .arg(blackMistakeCount)
            .arg(whiteMistakeCount)
    );
}

void OthelloWindow::showPassMessage()
{
    QMessageBox::information(
        this,
        "パス",
        "置ける場所がないためパスしました。\n"
        "次の手番: " + playerName(game.getCurrentPlayer())
    );
}

void OthelloWindow::showFinishMessage()
{
    int black = game.countStones(OthelloGame::Black);
    int white = game.countStones(OthelloGame::White);

    OthelloGame::Cell winner = game.getWinner();

    QString result;

    if (winner == OthelloGame::Black) {
        result = "黒の勝ちです！";
    } else if (winner == OthelloGame::White) {
        result = "白の勝ちです！";
    } else {
        result = "引き分けです！";
    }

    QMessageBox::information(
        this,
        "ゲーム終了",
        QString("黒: %1\n白: %2\n\n%3\n\nお手付き 黒: %4回  白: %5回")
            .arg(black)
            .arg(white)
            .arg(result)
            .arg(blackMistakeCount)
            .arg(whiteMistakeCount)
    );
}

QString OthelloWindow::playerName(OthelloGame::Cell player) const
{
    if (player == OthelloGame::Black) {
        return "黒";
    }

    if (player == OthelloGame::White) {
        return "白";
    }

    return "なし";
}
#include "OthelloWindow.h"
#include "OthelloAI.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>

OthelloWindow::OthelloWindow(QWidget *parent)
    : QMainWindow(parent),
      playMode(HumanVsHuman),
      blackMistakeCount(0),
      whiteMistakeCount(0)
{
    setupUI();

    game.reset();
    boardWidget->setGame(&game);

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

    modeComboBox = new QComboBox(this);
    modeComboBox->addItem("人対人");
    modeComboBox->addItem("CPU戦");

    resetButton = new QPushButton("リセット", this);

    topLayout->addWidget(turnLabel);
    topLayout->addStretch();
    topLayout->addWidget(scoreLabel);
    topLayout->addWidget(mistakeLabel);
    topLayout->addWidget(modeComboBox);
    topLayout->addWidget(resetButton);

    boardWidget = new OthelloBoard(this);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(boardWidget, 0, Qt::AlignCenter);

    connect(boardWidget, &OthelloBoard::cellClicked,
            this, &OthelloWindow::handleCellClicked);

    connect(resetButton, &QPushButton::clicked,
            this, &OthelloWindow::resetGame);

    connect(modeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OthelloWindow::handleModeChanged);

    setWindowTitle("オセロ");
    setFixedSize(680, 590);
}

void OthelloWindow::handleModeChanged(int index)
{
    playMode = index == 0 ? HumanVsHuman : HumanVsCpu;
    resetGame();
}

void OthelloWindow::handleCellClicked(int row, int col)
{
    if (isCpuTurn()) {
        return;
    }

    OthelloGame::Cell player = game.getCurrentPlayer();

    if (!game.placeStone(row, col)) {
        addMistake(player);
        updateStatus();
        return;
    }

    handleAfterMove();
}

void OthelloWindow::handleAfterMove()
{
    OthelloGame::GameState state = game.checkGameState();

    updateBoard();
    updateStatus();

    if (state == OthelloGame::Passed) {
        showPassMessage();
        updateBoard();
        updateStatus();
    } else if (state == OthelloGame::Finished) {
        showFinishMessage();
        return;
    }

    if (isCpuTurn()) {
        QTimer::singleShot(500, this, &OthelloWindow::runCpuTurn);
    }
}

bool OthelloWindow::isCpuTurn() const
{
    return playMode == HumanVsCpu
           && game.getCurrentPlayer() == OthelloGame::White;
}

void OthelloWindow::runCpuTurn()
{
    if (!isCpuTurn()) {
        return;
    }

    int row = -1;
    int col = -1;

    if (OthelloAI::chooseMove(game, row, col)) {
        game.placeStone(row, col);
    }

    handleAfterMove();
}

void OthelloWindow::resetGame()
{
    game.reset();

    blackMistakeCount = 0;
    whiteMistakeCount = 0;

    updateBoard();
    updateStatus();

    if (isCpuTurn()) {
        QTimer::singleShot(500, this, &OthelloWindow::runCpuTurn);
    }
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
    boardWidget->update();
}

void OthelloWindow::updateStatus()
{
    QString modeText = playMode == HumanVsHuman ? "人対人" : "CPU戦";

    turnLabel->setText(
        "モード: " + modeText + " / 手番: " + playerName(game.getCurrentPlayer())
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
        return playMode == HumanVsCpu ? "白 CPU" : "白";
    }

    return "なし";
}
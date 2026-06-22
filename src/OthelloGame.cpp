#include "OthelloGame.h"

#include <vector>
#include <utility>

OthelloGame::OthelloGame()
{
    reset();
}

void OthelloGame::reset()
{
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            board[row][col] = Empty;
        }
    }

    board[3][3] = White;
    board[3][4] = Black;
    board[4][3] = Black;
    board[4][4] = White;

    currentPlayer = Black;
}

OthelloGame::Cell OthelloGame::getCell(int row, int col) const
{
    if (!isInside(row, col)) {
        return Empty;
    }

    return board[row][col];
}

OthelloGame::Cell OthelloGame::getCurrentPlayer() const
{
    return currentPlayer;
}

bool OthelloGame::canPlace(int row, int col) const
{
    return canPlaceForPlayer(row, col, currentPlayer);
}

bool OthelloGame::placeStone(int row, int col)
{
    if (!canPlace(row, col)) {
        return false;
    }

    board[row][col] = currentPlayer;
    flipStones(row, col, currentPlayer);
    currentPlayer = opponent(currentPlayer);

    return true;
}

OthelloGame::GameState OthelloGame::checkGameState()
{
    if (hasValidMove(currentPlayer)) {
        return Playing;
    }

    Cell nextPlayer = opponent(currentPlayer);

    if (hasValidMove(nextPlayer)) {
        currentPlayer = nextPlayer;
        return Passed;
    }

    return Finished;
}

bool OthelloGame::hasValidMove(Cell player) const
{
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            if (canPlaceForPlayer(row, col, player)) {
                return true;
            }
        }
    }

    return false;
}

bool OthelloGame::isGameOver() const
{
    return !hasValidMove(Black) && !hasValidMove(White);
}

int OthelloGame::countStones(Cell player) const
{
    int count = 0;

    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            if (board[row][col] == player) {
                ++count;
            }
        }
    }

    return count;
}

OthelloGame::Cell OthelloGame::getWinner() const
{
    int black = countStones(Black);
    int white = countStones(White);

    if (black > white) return Black;
    if (white > black) return White;

    return Empty;
}

OthelloGame::Cell OthelloGame::opponent(Cell player)
{
    return player == Black ? White : Black;
}

bool OthelloGame::isInside(int row, int col) const
{
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE;
}

bool OthelloGame::canPlaceForPlayer(int row, int col, Cell player) const
{
    if (!isInside(row, col)) return false;
    if (board[row][col] != Empty) return false;

    Cell enemy = opponent(player);

    const int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},          { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1}
    };

    for (const auto &dir : directions) {
        int r = row + dir[0];
        int c = col + dir[1];

        bool foundEnemy = false;

        while (isInside(r, c) && board[r][c] == enemy) {
            foundEnemy = true;
            r += dir[0];
            c += dir[1];
        }

        if (foundEnemy && isInside(r, c) && board[r][c] == player) {
            return true;
        }
    }

    return false;
}

void OthelloGame::flipStones(int row, int col, Cell player)
{
    Cell enemy = opponent(player);

    const int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},          { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1}
    };

    for (const auto &dir : directions) {
        int r = row + dir[0];
        int c = col + dir[1];

        std::vector<std::pair<int, int>> stonesToFlip;

        while (isInside(r, c) && board[r][c] == enemy) {
            stonesToFlip.push_back({r, c});
            r += dir[0];
            c += dir[1];
        }

        if (isInside(r, c) && board[r][c] == player) {
            for (const auto &pos : stonesToFlip) {
                board[pos.first][pos.second] = player;
            }
        }
    }
}
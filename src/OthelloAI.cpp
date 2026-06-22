#include "OthelloAI.h"

bool OthelloAI::chooseMove(const OthelloGame &game, int &bestRow, int &bestCol)
{
    int bestScore = -1;
    bestRow = -1;
    bestCol = -1;

    for (int row = 0; row < OthelloGame::SIZE; ++row) {
        for (int col = 0; col < OthelloGame::SIZE; ++col) {
            if (game.canPlace(row, col)) {
                int score = evaluateMove(game, row, col);

                if (score > bestScore) {
                    bestScore = score;
                    bestRow = row;
                    bestCol = col;
                }
            }
        }
    }

    return bestRow != -1;
}

int OthelloAI::evaluateMove(const OthelloGame &game, int row, int col)
{
    OthelloGame copy = game;

    OthelloGame::Cell player = copy.getCurrentPlayer();
    int before = copy.countStones(player);

    copy.placeStone(row, col);

    int after = copy.countStones(player);
    int score = after - before;

    if ((row == 0 || row == 7) && (col == 0 || col == 7)) {
        score += 100;
    }

    return score;
}
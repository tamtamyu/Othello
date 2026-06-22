#include "OthelloAI.h"

bool OthelloAI::chooseMove(const OthelloGame &game, int &bestRow, int &bestCol)
{
    int bestScore = -100000;
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

    return bestRow != -1 && bestCol != -1;
}

int OthelloAI::evaluateMove(const OthelloGame &game, int row, int col)
{
    static const int POSITION_SCORE[8][8] = {
        {100, -20, 10,  5,  5, 10, -20, 100},
        {-20, -50, -2, -2, -2, -2, -50, -20},
        { 10,  -2, -1, -1, -1, -1,  -2,  10},
        {  5,  -2, -1, -1, -1, -1,  -2,   5},
        {  5,  -2, -1, -1, -1, -1,  -2,   5},
        { 10,  -2, -1, -1, -1, -1,  -2,  10},
        {-20, -50, -2, -2, -2, -2, -50, -20},
        {100, -20, 10,  5,  5, 10, -20, 100}
    };

    OthelloGame copy = game;

    OthelloGame::Cell player = copy.getCurrentPlayer();

    int before = copy.countStones(player);

    copy.placeStone(row, col);

    int after = copy.countStones(player);

    int flippedCount = after - before;

    int score = 0;

    score += POSITION_SCORE[row][col];
    score += flippedCount;

    return score;
}
#include "OthelloAI.h"

bool OthelloAI::chooseMove(const OthelloGame &game, int &bestRow, int &bestCol)
{
    static AIParameters params;
    static bool loaded = params.loadFromDefaultLocations();

    Q_UNUSED(loaded);

    int bestScore = -1000000;
    bestRow = -1;
    bestCol = -1;

    for (int row = 0; row < OthelloGame::SIZE; ++row) {
        for (int col = 0; col < OthelloGame::SIZE; ++col) {
            if (game.canPlace(row, col)) {
                int score = evaluateMove(game, row, col, params);

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

int OthelloAI::evaluateMove(const OthelloGame &game, int row, int col, const AIParameters &params)
{
    OthelloGame copy = game;

    OthelloGame::Cell player = copy.getCurrentPlayer();

    int before = copy.countStones(player);

    copy.placeStone(row, col);

    int after = copy.countStones(player);
    int flippedCount = after - before;

    int score = 0;

    score += params.positionScore(row, col);
    score += flippedCount * params.flipWeight();

    for (int r = 0; r < OthelloGame::SIZE; ++r) {
        for (int c = 0; c < OthelloGame::SIZE; ++c) {
            if (isCorner(r, c) && copy.canPlace(r, c)) {
                score += params.giveCornerPenalty();
            }
        }
    }

    int opponentMoves = 0;
    int ownMoves = 0;

    for (int r = 0; r < OthelloGame::SIZE; ++r) {
        for (int c = 0; c < OthelloGame::SIZE; ++c) {
            if (copy.canPlace(r, c)) {
                ++opponentMoves;
            }
        }
    }

    OthelloGame playerTurnCopy = copy;

    while (playerTurnCopy.getCurrentPlayer() != player) {
        playerTurnCopy.checkGameState();
        break;
    }

    for (int r = 0; r < OthelloGame::SIZE; ++r) {
        for (int c = 0; c < OthelloGame::SIZE; ++c) {
            if (copy.hasValidMove(player)) {
                if (copy.getCell(r, c) == OthelloGame::Empty) {
                    // 正確な自分の合法手数は次のLvで関数追加して改善する
                }
            }
        }
    }

    score -= opponentMoves * params.mobilityWeight();

    return score;
}

bool OthelloAI::isCorner(int row, int col)
{
    return (row == 0 || row == OthelloGame::SIZE - 1) &&
           (col == 0 || col == OthelloGame::SIZE - 1);
}
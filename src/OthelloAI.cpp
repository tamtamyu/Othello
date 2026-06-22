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
    OthelloGame::Cell enemy = OthelloGame::opponent(player);

    int before = copy.countStones(player);

    copy.placeStone(row, col);

    int after = copy.countStones(player);
    int flippedCount = after - before;

    int score = 0;

    score += params.positionScore(row, col);
    score += flippedCount * params.flipWeight();

    // Lv2: 自分が置いた直後、相手が角を取れるなら減点
    for (int r = 0; r < OthelloGame::SIZE; ++r) {
        for (int c = 0; c < OthelloGame::SIZE; ++c) {
            if (isCorner(r, c) && copy.canPlaceForPlayer(r, c, enemy)) {
                score += params.giveCornerPenalty();
            }
        }
    }

    // Lv3: 合法手数 Mobility
    // 自分の次の選択肢が多く、相手の選択肢が少ないほど高評価
    int ownMoves = copy.countValidMoves(player);
    int enemyMoves = copy.countValidMoves(enemy);

    score += (ownMoves - enemyMoves) * params.mobilityWeight();

    return score;
}

bool OthelloAI::isCorner(int row, int col)
{
    return (row == 0 || row == OthelloGame::SIZE - 1) &&
           (col == 0 || col == OthelloGame::SIZE - 1);
}
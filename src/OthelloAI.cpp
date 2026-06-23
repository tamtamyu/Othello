#include "OthelloAI.h"

bool OthelloAI::chooseMove(const OthelloGame &game, int &bestRow, int &bestCol)
{
    static AIParameters params;
    static bool loaded = params.loadFromDefaultLocations();

    Q_UNUSED(loaded);

    OthelloGame::Cell aiPlayer = game.getCurrentPlayer();

    int bestScore = -100000000;
    bestRow = -1;
    bestCol = -1;

    for (int row = 0; row < OthelloGame::SIZE; ++row) {
        for (int col = 0; col < OthelloGame::SIZE; ++col) {
            if (game.canPlace(row, col)) {
                OthelloGame copy = game;
                copy.placeStone(row, col);

                int score = minimax(copy, SEARCH_DEPTH - 1, aiPlayer, params);

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

int OthelloAI::minimax(OthelloGame game, int depth,
                       OthelloGame::Cell aiPlayer,
                       const AIParameters &params)
{
    OthelloGame::GameState state = game.checkGameState();

    if (state == OthelloGame::Finished || depth == 0) {
        return evaluateBoard(game, aiPlayer, params);
    }

    OthelloGame::Cell currentPlayer = game.getCurrentPlayer();

    bool maximizing = currentPlayer == aiPlayer;

    if (maximizing) {
        int bestScore = -100000000;

        for (int row = 0; row < OthelloGame::SIZE; ++row) {
            for (int col = 0; col < OthelloGame::SIZE; ++col) {
                if (game.canPlace(row, col)) {
                    OthelloGame copy = game;
                    copy.placeStone(row, col);

                    int score = minimax(copy, depth - 1, aiPlayer, params);

                    if (score > bestScore) {
                        bestScore = score;
                    }
                }
            }
        }

        return bestScore;
    } else {
        int bestScore = 100000000;

        for (int row = 0; row < OthelloGame::SIZE; ++row) {
            for (int col = 0; col < OthelloGame::SIZE; ++col) {
                if (game.canPlace(row, col)) {
                    OthelloGame copy = game;
                    copy.placeStone(row, col);

                    int score = minimax(copy, depth - 1, aiPlayer, params);

                    if (score < bestScore) {
                        bestScore = score;
                    }
                }
            }
        }

        return bestScore;
    }
}

int OthelloAI::evaluateBoard(const OthelloGame &game,
                             OthelloGame::Cell aiPlayer,
                             const AIParameters &params)
{
    OthelloGame::Cell enemy = OthelloGame::opponent(aiPlayer);

    int score = 0;

    int aiStones = game.countStones(aiPlayer);
    int enemyStones = game.countStones(enemy);

    score += (aiStones - enemyStones) * params.flipWeight();

    int aiMoves = game.countValidMoves(aiPlayer);
    int enemyMoves = game.countValidMoves(enemy);

    score += (aiMoves - enemyMoves) * params.mobilityWeight();

    for (int row = 0; row < OthelloGame::SIZE; ++row) {
        for (int col = 0; col < OthelloGame::SIZE; ++col) {
            OthelloGame::Cell cell = game.getCell(row, col);

            if (cell == aiPlayer) {
                score += params.positionScore(row, col);
            } else if (cell == enemy) {
                score -= params.positionScore(row, col);
            }
        }
    }

    for (int row = 0; row < OthelloGame::SIZE; ++row) {
        for (int col = 0; col < OthelloGame::SIZE; ++col) {
            if (isCorner(row, col)) {
                if (game.canPlaceForPlayer(row, col, enemy)) {
                    score += params.giveCornerPenalty();
                }

                if (game.canPlaceForPlayer(row, col, aiPlayer)) {
                    score -= params.giveCornerPenalty();
                }
            }
        }
    }

    if (game.isGameOver()) {
        OthelloGame::Cell winner = game.getWinner();

        if (winner == aiPlayer) {
            score += 100000;
        } else if (winner == enemy) {
            score -= 100000;
        }
    }

    return score;
}

bool OthelloAI::isCorner(int row, int col)
{
    return (row == 0 || row == OthelloGame::SIZE - 1) &&
           (col == 0 || col == OthelloGame::SIZE - 1);
}
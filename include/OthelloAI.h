#ifndef OTHELLOAI_H
#define OTHELLOAI_H

#include "OthelloGame.h"
#include "AIParameters.h"

class OthelloAI
{
public:
    static bool chooseMove(const OthelloGame &game, int &bestRow, int &bestCol);

private:
    static const int SEARCH_DEPTH = 4;
    static const int ENDGAME_EMPTY_THRESHOLD = 10;

    static int minimax(OthelloGame game,
                       int depth,
                       OthelloGame::Cell aiPlayer,
                       const AIParameters &params,
                       int alpha,
                       int beta);

    static int endgameSearch(OthelloGame game,
                             OthelloGame::Cell aiPlayer,
                             int alpha,
                             int beta);

    static int evaluateBoard(const OthelloGame &game,
                             OthelloGame::Cell aiPlayer,
                             const AIParameters &params);

    static int evaluateFinalResult(const OthelloGame &game,
                                   OthelloGame::Cell aiPlayer);

    static int countEmptyCells(const OthelloGame &game);

    static bool isCorner(int row, int col);
};

#endif
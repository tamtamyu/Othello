#ifndef OTHELLOAI_H
#define OTHELLOAI_H

#include "OthelloGame.h"
#include "AIParameters.h"

class OthelloAI
{
public:
    static bool chooseMove(const OthelloGame &game, int &bestRow, int &bestCol);

private:
    static const int SEARCH_DEPTH = 2;

    static int minimax(OthelloGame game, int depth,
                       OthelloGame::Cell aiPlayer,
                       const AIParameters &params);

    static int evaluateBoard(const OthelloGame &game,
                             OthelloGame::Cell aiPlayer,
                             const AIParameters &params);

    static bool isCorner(int row, int col);
};

#endif
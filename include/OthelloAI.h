#ifndef OTHELLOAI_H
#define OTHELLOAI_H

#include "OthelloGame.h"
#include "AIParameters.h"

class OthelloAI
{
public:
    static bool chooseMove(const OthelloGame &game, int &bestRow, int &bestCol);

private:
    static int evaluateMove(const OthelloGame &game, int row, int col, const AIParameters &params);
    static bool isCorner(int row, int col);
};

#endif
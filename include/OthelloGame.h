#ifndef OTHELLOGAME_H
#define OTHELLOGAME_H

class OthelloGame
{
public:
    static const int SIZE = 8;

    enum Cell {
        Empty,
        Black,
        White
    };

    enum GameState {
        Playing,
        Passed,
        Finished
    };

    OthelloGame();

    void reset();

    Cell getCell(int row, int col) const;
    Cell getCurrentPlayer() const;

    bool canPlace(int row, int col) const;
    bool canPlaceForPlayer(int row, int col, Cell player) const;

    bool placeStone(int row, int col);

    GameState checkGameState();

    bool hasValidMove(Cell player) const;
    int countValidMoves(Cell player) const;

    bool isGameOver() const;

    int countStones(Cell player) const;
    Cell getWinner() const;

    static Cell opponent(Cell player);

private:
    Cell board[SIZE][SIZE];
    Cell currentPlayer;

    bool isInside(int row, int col) const;
    void flipStones(int row, int col, Cell player);
};

#endif
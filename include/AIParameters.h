#ifndef AIPARAMETERS_H
#define AIPARAMETERS_H

#include <QString>

class AIParameters
{
public:
    static const int SIZE = 8;

    AIParameters();

    bool loadFromJson(const QString &filePath);
    bool loadFromDefaultLocations();

    int positionScore(int row, int col) const;
    int flipWeight() const;
    int mobilityWeight() const;
    int giveCornerPenalty() const;

private:
    int positionScoreTable[SIZE][SIZE];
    int flipWeightValue;
    int mobilityWeightValue;
    int giveCornerPenaltyValue;

    void setDefaultValues();
};

#endif
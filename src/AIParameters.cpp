#include "AIParameters.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDir>

AIParameters::AIParameters()
{
    setDefaultValues();
}

void AIParameters::setDefaultValues()
{
    int defaultPositionScore[SIZE][SIZE] = {
        {100, -20, 10,  5,  5, 10, -20, 100},
        {-20, -50, -2, -2, -2, -2, -50, -20},
        { 10,  -2, -1, -1, -1, -1,  -2,  10},
        {  5,  -2, -1, -1, -1, -1,  -2,   5},
        {  5,  -2, -1, -1, -1, -1,  -2,   5},
        { 10,  -2, -1, -1, -1, -1,  -2,  10},
        {-20, -50, -2, -2, -2, -2, -50, -20},
        {100, -20, 10,  5,  5, 10, -20, 100}
    };

    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            positionScoreTable[row][col] = defaultPositionScore[row][col];
        }
    }

    flipWeightValue = 1;
    mobilityWeightValue = 3;
    giveCornerPenaltyValue = -200;
}

bool AIParameters::loadFromDefaultLocations()
{
    QString appDir = QCoreApplication::applicationDirPath();

    QStringList paths = {
        "ai_training/ai_params.json",
        "../ai_training/ai_params.json",
        "../../ai_training/ai_params.json",
        appDir + "/ai_training/ai_params.json",
        appDir + "/../ai_training/ai_params.json",
        appDir + "/../../ai_training/ai_params.json"
    };

    for (const QString &path : paths) {
        if (loadFromJson(path)) {
            return true;
        }
    }

    return false;
}

bool AIParameters::loadFromJson(const QString &filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("flip_weight")) {
        flipWeightValue = obj["flip_weight"].toInt(flipWeightValue);
    }

    if (obj.contains("mobility_weight")) {
        mobilityWeightValue = obj["mobility_weight"].toInt(mobilityWeightValue);
    }

    if (obj.contains("give_corner_penalty")) {
        giveCornerPenaltyValue = obj["give_corner_penalty"].toInt(giveCornerPenaltyValue);
    }

    if (obj.contains("position_score") && obj["position_score"].isArray()) {
        QJsonArray boardArray = obj["position_score"].toArray();

        for (int row = 0; row < SIZE && row < boardArray.size(); ++row) {
            if (!boardArray[row].isArray()) {
                continue;
            }

            QJsonArray rowArray = boardArray[row].toArray();

            for (int col = 0; col < SIZE && col < rowArray.size(); ++col) {
                positionScoreTable[row][col] = rowArray[col].toInt(positionScoreTable[row][col]);
            }
        }
    }

    return true;
}

int AIParameters::positionScore(int row, int col) const
{
    return positionScoreTable[row][col];
}

int AIParameters::flipWeight() const
{
    return flipWeightValue;
}

int AIParameters::mobilityWeight() const
{
    return mobilityWeightValue;
}

int AIParameters::giveCornerPenalty() const
{
    return giveCornerPenaltyValue;
}
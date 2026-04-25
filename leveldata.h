#ifndef LEVELDATA_H
#define LEVELDATA_H

#include <QVector>
#include "gameobjects.h"

struct LevelData
{
    QVector<Role> circles;
    QVector<Role> squares;
    QVector<Spike> spikes;
    QVector<Block> blocks;
    QVector<MovingSpike> movingSpikes;

    int groundY;

    int doorX;
    int doorY;
    int doorW;
    int doorH;

    bool doorOpenAtStart;
};

LevelData buildLevelData(int level, int windowWidth);

#endif // LEVELDATA_H
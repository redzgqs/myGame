#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <QPoint>

struct Role
{
    int x;
    int y;
    int w;
    int h;
    int vx;
    int vy;
    bool alive;
    bool escaped;
    bool onGround;
    bool isCircle;
};

struct Spike
{
    QPoint a;
    QPoint b;
    QPoint c;
};

struct Block
{
    int x;
    int y;
    int w;
    int h;
};

#endif // GAMEOBJECTS_H
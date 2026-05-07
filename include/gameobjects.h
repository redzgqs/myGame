#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <QPoint>
#include <QRect>

struct Role
{
    double x;
    double y;
    double vx;
    double vy;
    int w;
    int h;
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

struct MovingSpike
{
    int x;
    int y;
    int count;
    int vx;
    int leftBound;
    int rightBound;
    int height;
};

struct HiddenSpike
{
    QPoint a;
    QPoint b;
    QPoint c;
    QRect triggerRect;
    bool visible;
};

#endif
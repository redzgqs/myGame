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
    int height;   // 新增：移动刺高度
};

struct HiddenSpike
{
    QPoint a;
    QPoint b;
    QPoint c;
    QRect triggerRect;   // 进入这个范围就触发
    bool visible;        // 是否已经出现
};

#endif // GAMEOBJECTS_H
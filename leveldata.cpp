#include "leveldata.h"

LevelData buildLevelData(int level, int windowWidth)
{
    LevelData data;

    data.groundY = 524;

    data.doorW = 50;
    data.doorH = 80;
    data.doorX = 820;
    data.doorY = data.groundY - data.doorH;

    auto makeRole = [&](int x, int y, bool isCircle)
    {
        Role r;
        r.x = x;
        r.y = y;
        r.w = 40;
        r.h = 40;
        r.vx = 0;
        r.vy = 0;
        r.alive = true;
        r.escaped = false;
        r.onGround = true;
        r.isCircle = isCircle;
        return r;
    };

    auto addSpike = [&](QPoint a, QPoint b, QPoint c)
    {
        Spike s;
        s.a = a;
        s.b = b;
        s.c = c;
        data.spikes.append(s);
    };

    auto addBlock = [&](int x, int y, int w, int h)
    {
        Block b;
        b.x = x;
        b.y = y;
        b.w = w;
        b.h = h;
        data.blocks.append(b);
    };

    // =========================
    // 第1关
    // =========================
    if (level == 1)
    {
        data.circles.append(makeRole(100, data.groundY - 40, true));
        data.squares.append(makeRole(220, data.groundY - 40, false));

        addSpike(QPoint(500, data.groundY), QPoint(520, data.groundY - 30), QPoint(540, data.groundY));
        addSpike(QPoint(540, data.groundY), QPoint(560, data.groundY - 30), QPoint(580, data.groundY));
    }
    // =========================
    // 第2关
    // =========================
    else if (level == 2)
    {
        data.squares.append(makeRole(120, data.groundY - 40, false));
        data.circles.append(makeRole(430, data.groundY - 40, true));
        data.squares.append(makeRole(680, data.groundY - 40, false));

        addSpike(QPoint(250, data.groundY), QPoint(270, data.groundY - 30), QPoint(290, data.groundY));
        addSpike(QPoint(290, data.groundY), QPoint(310, data.groundY - 30), QPoint(330, data.groundY));

        addSpike(QPoint(560, data.groundY), QPoint(580, data.groundY - 30), QPoint(600, data.groundY));
        addSpike(QPoint(600, data.groundY), QPoint(620, data.groundY - 30), QPoint(640, data.groundY));
    }
    // =========================
    // 第3关
    // =========================
    else if (level == 3)
    {
        addBlock(0, 250, windowWidth, 45);

        data.squares.append(makeRole(360, 250 - 40, false));

        data.circles.append(makeRole(80, data.groundY - 40, true));
        data.circles.append(makeRole(430, data.groundY - 40, true));

        addSpike(QPoint(680, 250), QPoint(700, 220), QPoint(720, 250));
        addSpike(QPoint(720, 250), QPoint(740, 220), QPoint(760, 250));
        addSpike(QPoint(760, 250), QPoint(780, 220), QPoint(800, 250));
        addSpike(QPoint(800, 250), QPoint(820, 220), QPoint(840, 250));

        addSpike(QPoint(340, data.groundY), QPoint(360, data.groundY - 30), QPoint(380, data.groundY));
        addSpike(QPoint(380, data.groundY), QPoint(400, data.groundY - 30), QPoint(420, data.groundY));

        addSpike(QPoint(580, data.groundY), QPoint(600, data.groundY - 30), QPoint(620, data.groundY));
        addSpike(QPoint(620, data.groundY), QPoint(640, data.groundY - 30), QPoint(660, data.groundY));

        data.doorX = 820;
        data.doorY = data.groundY - data.doorH;
    }

    return data;
}
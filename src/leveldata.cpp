#include "leveldata.h"

LevelData buildLevelData(int level, int windowWidth)
{
    LevelData data;
    data.doorOpenAtStart = false;

    data.groundY = 524;

    data.doorW = 40;
    data.doorH = 60;
    data.doorX = windowWidth - 20 - data.doorW;
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

    auto addMovingSpike = [&](int x, int y, int count, int vx, int leftBound, int rightBound, int height)
    {
        MovingSpike m;
        m.x = x;
        m.y = y;
        m.count = count;
        m.vx = vx;
        m.leftBound = leftBound;
        m.rightBound = rightBound;
        m.height = height;
        data.movingSpikes.append(m);
    };

    auto addHiddenSpike = [&](QPoint a, QPoint b, QPoint c, QRect triggerRect)
    {
        HiddenSpike h;
        h.a = a;
        h.b = b;
        h.c = c;
        h.triggerRect = triggerRect;
        h.visible = false;
        data.hiddenSpikes.append(h);
    };

    if (level == 1)
    {
        data.circles.append(makeRole(100, data.groundY - 40, true));
        data.squares.append(makeRole(220, data.groundY - 40, false));

        addSpike(QPoint(500, data.groundY), QPoint(520, data.groundY - 30), QPoint(540, data.groundY));
        addSpike(QPoint(540, data.groundY), QPoint(560, data.groundY - 30), QPoint(580, data.groundY));
    }
    else if (level == 2)
    {
        int blockW = 40;
        int spikeWidth = 80;
        int blockGap = 60;

        int leftSpikeX = 190;
        int rightSpikeX = 420;

        int leftSquareX = leftSpikeX - blockGap - blockW;
        int rightSquareX = rightSpikeX + spikeWidth + blockGap;
        int circleX = (leftSpikeX + spikeWidth + rightSpikeX - blockW) / 2;

        data.squares.append(makeRole(leftSquareX, data.groundY - 40, false));
        data.circles.append(makeRole(circleX, data.groundY - 40, true));
        data.squares.append(makeRole(rightSquareX, data.groundY - 40, false));

        addSpike(QPoint(leftSpikeX, data.groundY),
                 QPoint(leftSpikeX + 20, data.groundY - 30),
                 QPoint(leftSpikeX + 40, data.groundY));

        addSpike(QPoint(leftSpikeX + 40, data.groundY),
                 QPoint(leftSpikeX + 60, data.groundY - 30),
                 QPoint(leftSpikeX + 80, data.groundY));

        addSpike(QPoint(rightSpikeX, data.groundY),
                 QPoint(rightSpikeX + 20, data.groundY - 30),
                 QPoint(rightSpikeX + 40, data.groundY));

        addSpike(QPoint(rightSpikeX + 40, data.groundY),
                 QPoint(rightSpikeX + 60, data.groundY - 30),
                 QPoint(rightSpikeX + 80, data.groundY));
    }
    else if (level == 3)
    {
        addBlock(0, 250, windowWidth, 20);

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

        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;
    }
    else if (level == 4)
    {
        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;

        int stairW = 45;
        int gap = 55;

        int x1 = 120;
        int x2 = x1 + stairW + gap;
        int x3 = x2 + stairW + gap;
        int x4 = x3 + stairW + gap;
        int x5 = x4 + stairW + gap;
        int x6 = x5 + stairW + gap;

        int y1 = 470;
        int y2 = 420;
        int y3 = 370;
        int y4 = 320;
        int y5 = 270;
        int y6 = 220;

        addBlock(x1, y1, stairW, data.groundY - y1);
        addBlock(x2, y2, stairW, data.groundY - y2);
        addBlock(x3, y3, stairW, data.groundY - y3);
        addBlock(x4, y4, stairW, data.groundY - y4);
        addBlock(x5, y5, stairW, data.groundY - y5);
        addBlock(x6, y6, stairW, data.groundY - y6);

        data.circles.append(makeRole(40, data.groundY - 40, true));

        int centerOffset = (stairW - 40) / 2;

        data.squares.append(makeRole(x1 + centerOffset, y1 - 40, false));
        data.squares.append(makeRole(x2 + centerOffset, y2 - 40, false));
        data.squares.append(makeRole(x3 + centerOffset, y3 - 40, false));
        data.squares.append(makeRole(x4 + centerOffset, y4 - 40, false));
        data.squares.append(makeRole(x5 + centerOffset, y5 - 40, false));
        data.squares.append(makeRole(x6 + centerOffset, y6 - 40, false));

        auto addGapSpikes = [&](int leftBlockX)
        {
            int startX = leftBlockX + stairW + (gap - 40) / 2;
            addSpike(QPoint(startX, data.groundY), QPoint(startX + 10, data.groundY - 28), QPoint(startX + 20, data.groundY));
            addSpike(QPoint(startX + 20, data.groundY), QPoint(startX + 30, data.groundY - 28), QPoint(startX + 40, data.groundY));
        };

        addGapSpikes(x1);
        addGapSpikes(x2);
        addGapSpikes(x3);
        addGapSpikes(x4);
        addGapSpikes(x5);
    }
    else if (level == 5)
    {
        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;

        addBlock(170, 360, 240, 20);
        addBlock(500, 250, 240, 20);

        data.circles.append(makeRole(40, data.groundY - 40, true));
        data.circles.append(makeRole(185, 360 - 40, true));
        data.circles.append(makeRole(580, 250 - 40, true));

        data.squares.append(makeRole(310, data.groundY - 40, false));
        data.squares.append(makeRole(350, 360 - 40, false));

        addSpike(QPoint(230, data.groundY), QPoint(240, data.groundY - 28), QPoint(250, data.groundY));
        addSpike(QPoint(250, data.groundY), QPoint(260, data.groundY - 28), QPoint(270, data.groundY));
        addSpike(QPoint(270, data.groundY), QPoint(280, data.groundY - 28), QPoint(290, data.groundY));

        addSpike(QPoint(415, data.groundY), QPoint(425, data.groundY - 28), QPoint(435, data.groundY));
        addSpike(QPoint(435, data.groundY), QPoint(445, data.groundY - 28), QPoint(455, data.groundY));
        addSpike(QPoint(455, data.groundY), QPoint(465, data.groundY - 28), QPoint(475, data.groundY));
        addSpike(QPoint(475, data.groundY), QPoint(485, data.groundY - 28), QPoint(495, data.groundY));
        addSpike(QPoint(495, data.groundY), QPoint(505, data.groundY - 28), QPoint(515, data.groundY));

        addSpike(QPoint(670, 250), QPoint(680, 222), QPoint(690, 250));
        addSpike(QPoint(690, 250), QPoint(700, 222), QPoint(710, 250));
        addSpike(QPoint(710, 250), QPoint(720, 222), QPoint(730, 250));
    }
    else if (level == 6)
    {
        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;
        data.doorOpenAtStart = true;

        data.circles.append(makeRole(40, data.groundY - 40, true));

        addMovingSpike(220, data.groundY, 2, -4, 0, windowWidth, 36);
        addMovingSpike(500, data.groundY, 2, -4, 0, windowWidth, 36);
        addMovingSpike(780, data.groundY, 2, -4, 0, windowWidth, 36);
    }
    else if (level == 7)
    {
        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;
        data.doorOpenAtStart = false;

        addBlock(60, 60, 40, 350);
        addBlock(100, 390, 260, 20);
        addBlock(360, 330, 220, 80);
        addBlock(580, 390, 180, 20);
        addBlock(760, 330, 50, 80);

        data.circles.append(makeRole(390, 330 - 40, true));
        data.circles.append(makeRole(450, 330 - 40, true));
        data.circles.append(makeRole(510, 330 - 40, true));

        data.squares.append(makeRole(765, 330 - 40, false));

        addMovingSpike(220, 390, 2, -3, 100, 360, 36);
        addMovingSpike(620, 390, 2, -3, 580, 760, 36);

        addSpike(QPoint(710, 390), QPoint(720, 362), QPoint(730, 390));
        addSpike(QPoint(730, 390), QPoint(740, 362), QPoint(750, 390));
    }
    else if (level == 8)
    {
        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;
        data.doorOpenAtStart = true;

        data.circles.append(makeRole(40, data.groundY - 40, true));

        int hx = 470;
        QRect trigger(hx, data.groundY - 30, 80, 30);

        addHiddenSpike(
            QPoint(hx, data.groundY),
            QPoint(hx + 20, data.groundY - 30),
            QPoint(hx + 40, data.groundY),
            trigger
            );

        addHiddenSpike(
            QPoint(hx + 40, data.groundY),
            QPoint(hx + 60, data.groundY - 30),
            QPoint(hx + 80, data.groundY),
            trigger
            );
    }
    else if (level == 9)
    {
        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;
        data.doorOpenAtStart = true;

        int startX = 20;
        int spacing = 45;
        int endX = data.doorX - 50;

        for (int x = startX; x <= endX; x += spacing)
        {
            data.circles.append(makeRole(x, data.groundY - 40, true));
        }
    }

    return data;
}

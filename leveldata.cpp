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
        // 统一参数
        int blockW = 40;
        int spikeWidth = 80;   // 两个三角刺一共占 80
        int blockGap = 60;     // 方块到刺的距离：两边完全相等

        // 两组刺整体往左挪
        int leftSpikeX = 190;
        int rightSpikeX = 420;

        // 根据“方块到刺距离相等”反推方块位置
        int leftSquareX = leftSpikeX - blockGap - blockW;
        int rightSquareX = rightSpikeX + spikeWidth + blockGap;

        // 圆放在中间
        int circleX = (leftSpikeX + spikeWidth + rightSpikeX - blockW) / 2;

        // 左方块
        data.squares.append(makeRole(leftSquareX, data.groundY - 40, false));

        // 中间圆
        data.circles.append(makeRole(circleX, data.groundY - 40, true));

        // 右方块
        data.squares.append(makeRole(rightSquareX, data.groundY - 40, false));

        // 左侧刺组
        addSpike(QPoint(leftSpikeX, data.groundY),
                 QPoint(leftSpikeX + 20, data.groundY - 30),
                 QPoint(leftSpikeX + 40, data.groundY));

        addSpike(QPoint(leftSpikeX + 40, data.groundY),
                 QPoint(leftSpikeX + 60, data.groundY - 30),
                 QPoint(leftSpikeX + 80, data.groundY));

        // 右侧刺组
        addSpike(QPoint(rightSpikeX, data.groundY),
                 QPoint(rightSpikeX + 20, data.groundY - 30),
                 QPoint(rightSpikeX + 40, data.groundY));

        addSpike(QPoint(rightSpikeX + 40, data.groundY),
                 QPoint(rightSpikeX + 60, data.groundY - 30),
                 QPoint(rightSpikeX + 80, data.groundY));
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
    else if (level == 4)
    {
        // 门放回最右下角
        data.doorW = 50;
        data.doorH = 80;
        data.doorX = 820;
        data.doorY = data.groundY - data.doorH;

        // =========================
        // 可调参数
        // =========================
        int stairW = 60;   // 台阶宽度
        int gap    = 55;   // 台阶间距

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

        // =========================
        // 台阶
        // =========================
        addBlock(x1, y1, stairW, data.groundY - y1);
        addBlock(x2, y2, stairW, data.groundY - y2);
        addBlock(x3, y3, stairW, data.groundY - y3);
        addBlock(x4, y4, stairW, data.groundY - y4);
        addBlock(x5, y5, stairW, data.groundY - y5);
        addBlock(x6, y6, stairW, data.groundY - y6);

        // =========================
        // 圆
        // =========================
        data.circles.append(makeRole(40, data.groundY - 40, true));

        // =========================
        // 方块：全部放在台阶正中间
        // 方块宽度是40，所以居中公式是 x + (stairW - 40) / 2
        // =========================
        int centerOffset = (stairW - 40) / 2;

        data.squares.append(makeRole(x1 + centerOffset, y1 - 40, false));
        data.squares.append(makeRole(x2 + centerOffset, y2 - 40, false));
        data.squares.append(makeRole(x3 + centerOffset, y3 - 40, false));
        data.squares.append(makeRole(x4 + centerOffset, y4 - 40, false));
        data.squares.append(makeRole(x5 + centerOffset, y5 - 40, false));
        data.squares.append(makeRole(x6 + centerOffset, y6 - 40, false));

        // =========================
        // 刺：放在每两个台阶中间
        // =========================
        auto addGapSpikes = [&](int leftBlockX)
        {
            int startX = leftBlockX + stairW + (gap - 40) / 2;
            addSpike(QPoint(startX,      data.groundY), QPoint(startX + 10, data.groundY - 28), QPoint(startX + 20, data.groundY));
            addSpike(QPoint(startX + 20, data.groundY), QPoint(startX + 30, data.groundY - 28), QPoint(startX + 40, data.groundY));
        };

        addGapSpikes(x1);
        addGapSpikes(x2);
        addGapSpikes(x3);
        addGapSpikes(x4);
        addGapSpikes(x5);
    }

    return data;
}
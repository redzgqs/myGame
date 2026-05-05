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
        // 门放回最右下角
        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;

        // =========================
        // 可调参数
        // =========================
        int stairW = 45;   // 台阶宽度
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
    else if (level == 5)
    {
        // =========================
        // 第5关：三层三圆三方块
        // 门仍然在右下角
        // =========================

        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;

        // -------------------------
        // 平台
        // -------------------------
        // 中层长平台
        addBlock(170, 360, 240, 20);

        // 上层长平台（加长，方便同时放圆和方块）
        addBlock(500, 250, 240, 20);

        // -------------------------
        // 圆
        // -------------------------
        // 圆1：左下角地面
        data.circles.append(makeRole(40, data.groundY - 40, true));

        // 圆2：中层平台左边
        data.circles.append(makeRole(185, 360 - 40, true));

        // 圆3：上层平台左边
        data.circles.append(makeRole(580, 250 - 40, true));

        // -------------------------
        // 方块
        // -------------------------
        // 方块1：地面中间，先往左可掉进左侧三连刺
        data.squares.append(makeRole(310, data.groundY - 40, false));

        // 方块2：中层平台右边，往右会掉下去，再落进地面四连刺
        data.squares.append(makeRole(350, 360 - 40, false));



        // -------------------------
        // 刺
        // -------------------------

        // 地面左侧三连刺：用于清掉方块1
        addSpike(QPoint(230, data.groundY), QPoint(240, data.groundY - 28), QPoint(250, data.groundY));
        addSpike(QPoint(250, data.groundY), QPoint(260, data.groundY - 28), QPoint(270, data.groundY));
        addSpike(QPoint(270, data.groundY), QPoint(280, data.groundY - 28), QPoint(290, data.groundY));

        // 地面中右四连刺：用于清掉方块2，也增加圆回收难度
        addSpike(QPoint(415, data.groundY), QPoint(425, data.groundY - 28), QPoint(435, data.groundY));
        addSpike(QPoint(435, data.groundY), QPoint(445, data.groundY - 28), QPoint(455, data.groundY));
        addSpike(QPoint(455, data.groundY), QPoint(465, data.groundY - 28), QPoint(475, data.groundY));
        addSpike(QPoint(475, data.groundY), QPoint(485, data.groundY - 28), QPoint(495, data.groundY));
        addSpike(QPoint(495, data.groundY), QPoint(505, data.groundY - 28), QPoint(515, data.groundY));

        // 上层右侧三连刺：用于清掉方块3
        addSpike(QPoint(670, 250), QPoint(680, 222), QPoint(690, 250));
        addSpike(QPoint(690, 250), QPoint(700, 222), QPoint(710, 250));
        addSpike(QPoint(710, 250), QPoint(720, 222), QPoint(730, 250));
    }
    else if (level == 6)
    {
        // 第6关：移动刺
        // 一个圆，没有方块，门一开始就开着

        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;
        data.doorOpenAtStart = true;

        // 一个圆，最左边
        data.circles.append(makeRole(40, data.groundY - 40, true));

        // 三个移动刺，初始都向左移动
        // count = 2 表示每个移动刺由两个小三角组成
        // rightBound 设成 780，避免它们直接堵住最右边门口
        addMovingSpike(220, data.groundY, 2, -4, 0, windowWidth, 36);
        addMovingSpike(500, data.groundY, 2, -4, 0, windowWidth, 36);
        addMovingSpike(780, data.groundY, 2, -4, 0, windowWidth, 36);
    }
    else if (level == 7)
    {
        // =========================
        // 第7关：中台三圆 + 两侧凹槽
        // 左边移动刺，右边移动刺+静态刺
        // 门在右下角
        // =========================

        // 门
        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;
        data.doorOpenAtStart = false;   // 有方块，初始关门

        // -------------------------
        // 连成一体的上层结构
        // -------------------------

        // 左侧高墙：只连到上层，不再插到底部地面
        // 顶部 y=60，底部到 410，正好和上层平台连接
        addBlock(60, 60, 40, 350);

        // 左侧上层平台（低）
        addBlock(100, 390, 260, 20);

        // 中央大平台（高）
        addBlock(360, 330, 220, 80);

        // 右侧上层平台（低）
        addBlock(580, 390, 180, 20);

        // 最右小高台（高）
        addBlock(760, 330, 50, 80);

        // -------------------------
        // 三个圆：都在中央大平台上
        // -------------------------
        data.circles.append(makeRole(390, 330 - 40, true));
        data.circles.append(makeRole(450, 330 - 40, true));
        data.circles.append(makeRole(510, 330 - 40, true));

        // -------------------------
        // 一个方块：在最右小高台上
        // -------------------------
        data.squares.append(makeRole(765, 330 - 40, false));

        // -------------------------
        // 左侧移动刺：在左侧低平台上移动
        // -------------------------
        addMovingSpike(220, 390, 2, -3, 100, 360, 36);

        // -------------------------
        // 右侧移动刺：在右侧低平台上移动
        // -------------------------
        addMovingSpike(620, 390, 2, -3, 580, 760, 36);

        // -------------------------
        // 右侧静态刺：放在右侧低平台靠右位置
        // -------------------------
        addSpike(QPoint(710, 390), QPoint(720, 362), QPoint(730, 390));
        addSpike(QPoint(730, 390), QPoint(740, 362), QPoint(750, 390));
    }
    else if (level == 8)
    {
        // =========================
        // 第8关：隐藏双刺
        // 圆在最左边，门在最右边
        // 门一开始就开着
        // =========================

        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;
        data.doorOpenAtStart = true;

        // 一个圆，最左边
        data.circles.append(makeRole(40, data.groundY - 40, true));

        // 隐藏双刺：和 level 1 类似，两根并排
        int hx = 470;

        // 触发范围就按这组双刺的整体范围来
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
        // =========================
        // 第9关：最终收尾关
        // 没有方块、没有刺、门一开始就是开的
        // 一排很多圆，全部进门就通关
        // =========================

        data.doorW = 40;
        data.doorH = 60;
        data.doorX = windowWidth - 20 - data.doorW;
        data.doorY = data.groundY - data.doorH;
        data.doorOpenAtStart = true;

        // 一排很多真奶龙，铺满地面，直到门前
        int startX = 20;                 // 最左起点
        int spacing = 45;                // 相邻圆之间的间距
        int endX = data.doorX - 50;      // 门前留一点空间

        for (int x = startX; x <= endX; x += spacing)
        {
            data.circles.append(makeRole(x, data.groundY - 40, true));
        }
    }

    return data;
}
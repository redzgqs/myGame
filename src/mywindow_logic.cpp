#include "mywindow.h"
#include <QMessageBox>
#include <cmath>

void MyWindow::scheduleNextLevel()
{
    if (waitingNextLevel)
        return;

    waitingNextLevel = true;
    timer->stop();

    update();              // 关键：立刻请求重绘
    clearTimer->start(1000);
}

QRect MyWindow::roleRect(const Role &role) const
{
    return QRect((int)std::round(role.x),
                 (int)std::round(role.y),
                 role.w,
                 role.h);
}


QRect MyWindow::blockRect(const Block &block) const
{
    return QRect(block.x, block.y, block.w, block.h);
}

void MyWindow::updateRole(Role &role, int dir)
{
    if (!role.alive || role.escaped)
        return;

    const double EPS = 0.8;

    // 1. 左右移动
    role.vx = dir * moveSpeed;
    role.x += role.vx;

    if (role.x < 0)
        role.x = 0;
    if (role.x + role.w > width())
        role.x = width() - role.w;

    QRect r = roleRect(role);

    // 左右碰墙/平台
    for (int i = 0; i < blocks.size(); i++)
    {
        QRect b = blockRect(blocks[i]);
        if (r.intersects(b))
        {
            if (role.vx > 0)
                role.x = blocks[i].x - role.w;
            else if (role.vx < 0)
                role.x = blocks[i].x + blocks[i].w;

            r = roleRect(role);
        }
    }

    // 2. 垂直移动
    double oldY = role.y;

    role.vy += gravity;
    role.y += role.vy;
    role.onGround = false;

    r = roleRect(role);

    // 与平台上下碰撞
    for (int i = 0; i < blocks.size(); i++)
    {
        QRect b = blockRect(blocks[i]);

        if (r.intersects(b))
        {
            // 从上方落到平台上
            if (oldY + role.h <= blocks[i].y + EPS)
            {
                role.y = blocks[i].y - role.h;
                role.vy = 0.0;
                role.onGround = true;
            }
            // 从下方顶到平台
            else if (oldY >= blocks[i].y + blocks[i].h - EPS)
            {
                role.y = blocks[i].y + blocks[i].h;
                role.vy = 0.0;
            }

            r = roleRect(role);
        }
    }

    // 与底部地面碰撞
    if (role.y + role.h >= groundY - EPS)
    {
        role.y = groundY - role.h;
        role.vy = 0.0;
        role.onGround = true;
    }
}


bool MyWindow::pointInTriangle(QPoint p, QPoint a, QPoint b, QPoint c)
{
    auto sign = [](QPoint p1, QPoint p2, QPoint p3)
    {
        return (p1.x() - p3.x()) * (p2.y() - p3.y()) -
               (p2.x() - p3.x()) * (p1.y() - p3.y());
    };

    bool b1 = sign(p, a, b) < 0;
    bool b2 = sign(p, b, c) < 0;
    bool b3 = sign(p, c, a) < 0;

    return (b1 == b2) && (b2 == b3);
}

bool MyWindow::roleHitSpike(const Role &role)
{
    if (!role.alive || role.escaped)
        return false;

    QPoint p1(role.x + 8,          role.y + role.h - 1);
    QPoint p2(role.x + role.w / 2, role.y + role.h - 1);
    QPoint p3(role.x + role.w - 8, role.y + role.h - 1);

    // 静态刺
    for (int i = 0; i < spikes.size(); i++)
    {
        if (pointInTriangle(p1, spikes[i].a, spikes[i].b, spikes[i].c)) return true;
        if (pointInTriangle(p2, spikes[i].a, spikes[i].b, spikes[i].c)) return true;
        if (pointInTriangle(p3, spikes[i].a, spikes[i].b, spikes[i].c)) return true;
    }

    // 移动刺
    for (int i = 0; i < movingSpikes.size(); i++)
    {
        for (int j = 0; j < movingSpikes[i].count; j++)
        {
            int sx = movingSpikes[i].x + j * 20;
            int sy = movingSpikes[i].y;

            int h = movingSpikes[i].height;

            QPoint a(sx, sy);
            QPoint b(sx + 10, sy - h);
            QPoint c(sx + 20, sy);

            if (pointInTriangle(p1, a, b, c)) return true;
            if (pointInTriangle(p2, a, b, c)) return true;
            if (pointInTriangle(p3, a, b, c)) return true;
        }
    }

    // 隐藏刺（只有 visible == true 才判定）
    for (int i = 0; i < hiddenSpikes.size(); i++)
    {
        if (!hiddenSpikes[i].visible)
            continue;

        if (pointInTriangle(p1, hiddenSpikes[i].a, hiddenSpikes[i].b, hiddenSpikes[i].c)) return true;
        if (pointInTriangle(p2, hiddenSpikes[i].a, hiddenSpikes[i].b, hiddenSpikes[i].c)) return true;
        if (pointInTriangle(p3, hiddenSpikes[i].a, hiddenSpikes[i].b, hiddenSpikes[i].c)) return true;
    }

    return false;
}

bool MyWindow::circlesHitSquares() const
{
    for (int i = 0; i < circles.size(); i++)
    {
        if (!circles[i].alive || circles[i].escaped)
            continue;

        QRect cRect = roleRect(circles[i]);

        for (int j = 0; j < squares.size(); j++)
        {
            if (!squares[j].alive)
                continue;

            QRect sRect = roleRect(squares[j]);

            if (cRect.intersects(sRect))
                return true;
        }
    }

    return false;
}


bool MyWindow::allSquaresDead() const
{
    for (int i = 0; i < squares.size(); i++)
    {
        if (squares[i].alive)
            return false;
    }
    return true;
}


void MyWindow::killCircleAndReset(int index)
{
    if (index < 0 || index >= circles.size())
        return;

    circles[index].alive = false;
    circles[index].vx = 0;
    circles[index].vy = 0;

    update();          // 先刷新一帧，让圆消失
    scheduleReset();   // 再延迟重开
}

void MyWindow::scheduleReset()
{
    if (waitingReset)
        return;

    waitingReset = true;
    timer->stop();
    resetTimer->start(1000);   // 1秒后重开
}

bool MyWindow::allCirclesEscaped() const
{
    for (int i = 0; i < circles.size(); i++)
    {
        if (circles[i].alive && !circles[i].escaped)
            return false;
    }
    return true;
}


void MyWindow::nextLevel()
{
    currentLevel++;

    if (currentLevel > 9)
    {
        showFinishScene();
        return;
    }

    loadLevel(currentLevel);
}

void MyWindow::updateMovingSpikes()
{
    for (int i = 0; i < movingSpikes.size(); i++)
    {
        movingSpikes[i].x += movingSpikes[i].vx;

        int totalWidth = movingSpikes[i].count * 20;

        if (movingSpikes[i].x < movingSpikes[i].leftBound)
        {
            movingSpikes[i].x = movingSpikes[i].leftBound;
            movingSpikes[i].vx = -movingSpikes[i].vx;
        }

        if (movingSpikes[i].x + totalWidth > movingSpikes[i].rightBound)
        {
            movingSpikes[i].x = movingSpikes[i].rightBound - totalWidth;
            movingSpikes[i].vx = -movingSpikes[i].vx;
        }
    }
}

void MyWindow::updateHiddenSpikes()
{
    for (int i = 0; i < hiddenSpikes.size(); i++)
    {
        if (hiddenSpikes[i].visible)
            continue;

        for (int j = 0; j < circles.size(); j++)
        {
            if (!circles[j].alive || circles[j].escaped)
                continue;

            if (roleRect(circles[j]).intersects(hiddenSpikes[i].triggerRect))
            {
                hiddenSpikes[i].visible = true;
                return;   // 一次触发一个就够了
            }
        }
    }
}



void MyWindow::updateGame()
{
    if (sceneState != GameScene || waitingReset || waitingNextLevel)
    {
        update();
        return;
    }

    int dir = 0;
    if (keyLeft && !keyRight)
        dir = -1;
    else if (keyRight && !keyLeft)
        dir = 1;

    // 更新所有圆
    for (int i = 0; i < circles.size(); i++)
    {
        updateRole(circles[i], dir);
    }

    // 更新所有方块
    for (int i = 0; i < squares.size(); i++)
    {
        updateRole(squares[i], dir);
    }

    if (!movingSpikes.isEmpty())
    {
        if (!movingSpikesStarted)
        {
            movingSpikeDelayFrames++;

            // 约 1 秒（定时器 16ms 一帧，大约 60 帧）
            if (movingSpikeDelayFrames >= 60)
            {
                movingSpikesStarted = true;
            }
        }
        else
        {
            updateMovingSpikes();
        }
    }

    if (jumpBufferFrames > 0)
    {
        bool jumped = false;

        for (int i = 0; i < circles.size(); i++)
        {
            if (circles[i].alive && !circles[i].escaped && circles[i].onGround)
            {
                circles[i].vy = -jumpSpeed;
                circles[i].onGround = false;
                jumped = true;
            }
        }

        for (int i = 0; i < squares.size(); i++)
        {
            if (squares[i].alive && squares[i].onGround)
            {
                squares[i].vy = -jumpSpeed;
                squares[i].onGround = false;
                jumped = true;
            }
        }

        if (jumped)
            jumpBufferFrames = 0;
        else
            jumpBufferFrames--;
    }

    updateHiddenSpikes();

    // 圆和方块重叠：失败重开
    for (int i = 0; i < circles.size(); i++)
    {
        if (!circles[i].alive || circles[i].escaped)
            continue;

        QRect cRect = roleRect(circles[i]);

        for (int j = 0; j < squares.size(); j++)
        {
            if (!squares[j].alive)
                continue;

            QRect sRect = roleRect(squares[j]);

            if (cRect.intersects(sRect))
            {
                killCircleAndReset(i);
                return;
            }
        }
    }

    // 圆碰刺：失败重开
    for (int i = 0; i < circles.size(); i++)
    {
        if (circles[i].alive && !circles[i].escaped && roleHitSpike(circles[i]))
        {
            killCircleAndReset(i);
            return;
        }
    }

    // 方块碰刺：死亡
    for (int i = 0; i < squares.size(); i++)
    {
        if (squares[i].alive && roleHitSpike(squares[i]))
        {
            squares[i].alive = false;
        }
    }

    // 所有方块都死了，门打开
    doorOpen = doorOpenAtStart || allSquaresDead();

    // all活着的圆进入门：进入下一关
    if (doorOpen)
    {
        QRect doorRect(doorX, doorY, doorW, doorH);

        for (int i = 0; i < circles.size(); i++)
        {
            if (circles[i].alive && !circles[i].escaped)
            {
                if (roleRect(circles[i]).intersects(doorRect))
                {
                    circles[i].escaped = true;
                }
            }
        }

        if (allCirclesEscaped())
        {
            // 最后一关：仍然进入最终通关界面
            if (currentLevel >= totalLevels)
            {
                showFinishScene();
            }
            else
            {
                scheduleNextLevel();
            }

            return;
        }
    }

    update();
}

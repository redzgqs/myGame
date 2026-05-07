#include "mywindow.h"

#include <cmath>

QRect MyWindow::roleRect(const Role &role) const
{
    return QRect(
        static_cast<int>(std::round(role.x)),
        static_cast<int>(std::round(role.y)),
        role.w,
        role.h
        );
}

QRect MyWindow::blockRect(const Block &block) const
{
    return QRect(block.x, block.y, block.w, block.h);
}

void MyWindow::updateRole(Role &role, int dir)
{
    if (!role.alive || role.escaped)
    {
        return;
    }

    const double EPS = 0.8;

    role.vx = dir * moveSpeed;
    role.x += role.vx;

    if (role.x < 0)
    {
        role.x = 0;
    }
    if (role.x + role.w > width())
    {
        role.x = width() - role.w;
    }

    QRect r = roleRect(role);

    for (int i = 0; i < blocks.size(); i++)
    {
        QRect b = blockRect(blocks[i]);
        if (r.intersects(b))
        {
            if (role.vx > 0)
            {
                role.x = blocks[i].x - role.w;
            }
            else if (role.vx < 0)
            {
                role.x = blocks[i].x + blocks[i].w;
            }

            r = roleRect(role);
        }
    }

    double oldY = role.y;

    role.vy += gravity;
    role.y += role.vy;
    role.onGround = false;

    r = roleRect(role);

    for (int i = 0; i < blocks.size(); i++)
    {
        QRect b = blockRect(blocks[i]);
        if (r.intersects(b))
        {
            if (oldY + role.h <= blocks[i].y + EPS)
            {
                role.y = blocks[i].y - role.h;
                role.vy = 0.0;
                role.onGround = true;
            }
            else if (oldY >= blocks[i].y + blocks[i].h - EPS)
            {
                role.y = blocks[i].y + blocks[i].h;
                role.vy = 0.0;
            }

            r = roleRect(role);
        }
    }

    if (role.y + role.h >= groundY - EPS)
    {
        role.y = groundY - role.h;
        role.vy = 0.0;
        role.onGround = true;
    }
}

void MyWindow::updateMovingSpikes()
{
    for (int i = 0; i < movingSpikes.size(); i++)
    {
        movingSpikes[i].x += movingSpikes[i].vx;

        const int totalWidth = movingSpikes[i].count * 20;

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
        {
            continue;
        }

        for (int j = 0; j < circles.size(); j++)
        {
            if (!circles[j].alive || circles[j].escaped)
            {
                continue;
            }

            if (roleRect(circles[j]).intersects(hiddenSpikes[i].triggerRect))
            {
                hiddenSpikes[i].visible = true;
                return;
            }
        }
    }
}

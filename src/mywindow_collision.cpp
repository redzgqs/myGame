#include "mywindow.h"

bool MyWindow::pointInTriangle(QPoint p, QPoint a, QPoint b, QPoint c)
{
    auto sign = [](QPoint p1, QPoint p2, QPoint p3) {
        return (p1.x() - p3.x()) * (p2.y() - p3.y())
        - (p2.x() - p3.x()) * (p1.y() - p3.y());
    };

    const bool b1 = sign(p, a, b) < 0;
    const bool b2 = sign(p, b, c) < 0;
    const bool b3 = sign(p, c, a) < 0;

    return (b1 == b2) && (b2 == b3);
}

bool MyWindow::roleHitSpike(const Role &role)
{
    if (!role.alive || role.escaped)
    {
        return false;
    }

    QPoint p1(role.x + 8, role.y + role.h - 1);
    QPoint p2(role.x + role.w / 2, role.y + role.h - 1);
    QPoint p3(role.x + role.w - 8, role.y + role.h - 1);

    for (int i = 0; i < spikes.size(); i++)
    {
        if (pointInTriangle(p1, spikes[i].a, spikes[i].b, spikes[i].c)) return true;
        if (pointInTriangle(p2, spikes[i].a, spikes[i].b, spikes[i].c)) return true;
        if (pointInTriangle(p3, spikes[i].a, spikes[i].b, spikes[i].c)) return true;
    }

    for (int i = 0; i < movingSpikes.size(); i++)
    {
        for (int j = 0; j < movingSpikes[i].count; j++)
        {
            const int sx = movingSpikes[i].x + j * 20;
            const int sy = movingSpikes[i].y;
            const int h = movingSpikes[i].height;

            QPoint a(sx, sy);
            QPoint b(sx + 10, sy - h);
            QPoint c(sx + 20, sy);

            if (pointInTriangle(p1, a, b, c)) return true;
            if (pointInTriangle(p2, a, b, c)) return true;
            if (pointInTriangle(p3, a, b, c)) return true;
        }
    }

    for (int i = 0; i < hiddenSpikes.size(); i++)
    {
        if (!hiddenSpikes[i].visible)
        {
            continue;
        }

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
        {
            continue;
        }

        QRect cRect = roleRect(circles[i]);

        for (int j = 0; j < squares.size(); j++)
        {
            if (!squares[j].alive)
            {
                continue;
            }

            QRect sRect = roleRect(squares[j]);
            if (cRect.intersects(sRect))
            {
                return true;
            }
        }
    }

    return false;
}

bool MyWindow::allSquaresDead() const
{
    for (int i = 0; i < squares.size(); i++)
    {
        if (squares[i].alive)
        {
            return false;
        }
    }
    return true;
}

bool MyWindow::allCirclesEscaped() const
{
    for (int i = 0; i < circles.size(); i++)
    {
        if (circles[i].alive && !circles[i].escaped)
        {
            return false;
        }
    }
    return true;
}

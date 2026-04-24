#include "mywindow.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QDebug>
#include <QMessageBox>
#include <QPolygon>

MyWindow::MyWindow(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(900, 600);
    setFocusPolicy(Qt::StrongFocus);

    bg.load(":/images/background.png");

    timer = new QTimer(this);

    initGame();

    connect(timer, &QTimer::timeout, this, &MyWindow::updateGame);
    timer->start(16);
}

MyWindow::~MyWindow()
{
}

void MyWindow::initGame()
{
    keyLeft = false;
    keyRight = false;
    keyUp = false;

    moveSpeed = 5;
    jumpSpeed = 20;
    gravity = 1;

    currentLevel = 1;
    loadLevel(currentLevel);
}

void MyWindow::resetGame()
{
    keyLeft = false;
    keyRight = false;
    keyUp = false;

    loadLevel(currentLevel);
}


void MyWindow::loadLevel(int level)
{
    keyLeft = false;
    keyRight = false;
    keyUp = false;

    circles.clear();
    squares.clear();
    spikes.clear();

    groundY = 524;

    doorOpen = false;
    doorW = 50;
    doorH = 80;
    doorX = 820;
    doorY = groundY - doorH;

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
        spikes.append(s);
    };

    // =========================
    // 第1关
    // =========================
    if (level == 1)
    {
        circles.append(makeRole(100, groundY - 40, true));
        squares.append(makeRole(220, groundY - 40, false));

        addSpike(QPoint(500, groundY), QPoint(520, groundY - 30), QPoint(540, groundY));
        addSpike(QPoint(540, groundY), QPoint(560, groundY - 30), QPoint(580, groundY));
    }
    // =========================
    // 第2关
    // 一个圆，两个方块
    // =========================
    else if (level == 2)
    {
        int leftSquareX = 120;
        int circleX = 430;
        int rightSquareX = 680;

        int leftSpikeX = 250;
        int rightSpikeX = 560;

        doorX = 820;

        squares.append(makeRole(leftSquareX, groundY - 40, false));
        circles.append(makeRole(circleX, groundY - 40, true));
        squares.append(makeRole(rightSquareX, groundY - 40, false));

        addSpike(QPoint(leftSpikeX, groundY), QPoint(leftSpikeX + 20, groundY - 30), QPoint(leftSpikeX + 40, groundY));
        addSpike(QPoint(leftSpikeX + 40, groundY), QPoint(leftSpikeX + 60, groundY - 30), QPoint(leftSpikeX + 80, groundY));

        addSpike(QPoint(rightSpikeX, groundY), QPoint(rightSpikeX + 20, groundY - 30), QPoint(rightSpikeX + 40, groundY));
        addSpike(QPoint(rightSpikeX + 40, groundY), QPoint(rightSpikeX + 60, groundY - 30), QPoint(rightSpikeX + 80, groundY));
    }
}

void MyWindow::nextLevel()
{
    currentLevel++;

    if (currentLevel > 2)
    {
        currentLevel = 1;
        QMessageBox::information(this, "游戏完成", "恭喜你完成了全部关卡！");
    }

    loadLevel(currentLevel);
}


QRect MyWindow::roleRect(const Role &role) const
{
    return QRect(role.x, role.y, role.w, role.h);
}

void MyWindow::drawRole(QPainter &painter, const Role &role)
{
    if (!role.alive)
        return;

    if (role.isCircle)
    {
        painter.setBrush(Qt::blue);
        painter.drawEllipse(role.x, role.y, role.w, role.h);
    }
    else
    {
        painter.setBrush(Qt::darkGreen);
        painter.drawRect(role.x, role.y, role.w, role.h);
    }
}

void MyWindow::updateRole(Role &role, int dir)
{
    if (!role.alive)
        return;

    // 左右移动
    role.vx = dir * moveSpeed;
    role.x += role.vx;

    // 左右边界
    if (role.x < 0)
        role.x = 0;
    if (role.x + role.w > width())
        role.x = width() - role.w;

    // 重力
    role.vy += gravity;
    role.y += role.vy;

    // 地面碰撞
    if (role.y + role.h >= groundY)
    {
        role.y = groundY - role.h;
        role.vy = 0;
        role.onGround = true;
    }
    else
    {
        role.onGround = false;
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
    if (!role.alive)
        return false;

    QPoint p1(role.x + 8,          role.y + role.h - 1);
    QPoint p2(role.x + role.w / 2, role.y + role.h - 1);
    QPoint p3(role.x + role.w - 8, role.y + role.h - 1);

    for (int i = 0; i < spikes.size(); i++)
    {
        if (pointInTriangle(p1, spikes[i].a, spikes[i].b, spikes[i].c)) return true;
        if (pointInTriangle(p2, spikes[i].a, spikes[i].b, spikes[i].c)) return true;
        if (pointInTriangle(p3, spikes[i].a, spikes[i].b, spikes[i].c)) return true;
    }

    return false;
}

bool MyWindow::circlesHitSquares() const
{
    for (int i = 0; i < circles.size(); i++)
    {
        if (!circles[i].alive)
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

void MyWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 背景
    painter.drawPixmap(rect(), bg);

    // 刺
    painter.setBrush(Qt::red);
    for (int i = 0; i < spikes.size(); i++)
    {
        QPolygon spikePoly;
        spikePoly << spikes[i].a << spikes[i].b << spikes[i].c;
        painter.drawPolygon(spikePoly);
    }

    // 画所有圆
    for (int i = 0; i < circles.size(); i++)
    {
        drawRole(painter, circles[i]);
    }

    // 画所有方块
    for (int i = 0; i < squares.size(); i++)
    {
        drawRole(painter, squares[i]);
    }

    // 门
    if (doorOpen)
    {
        painter.setBrush(QColor(139, 69, 19));
        painter.drawRect(doorX, doorY, doorW, doorH);

        painter.setBrush(Qt::yellow);
        painter.drawEllipse(doorX + doorW - 12, doorY + doorH / 2, 6, 6);
    }

    // 关卡
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(20, 30, QString("Level %1").arg(currentLevel));

}

void MyWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    if (event->key() == Qt::Key_Left)
    {
        keyLeft = true;
    }
    else if (event->key() == Qt::Key_Right)
    {
        keyRight = true;
    }
    else if (event->key() == Qt::Key_Up)
    {
        keyUp = true;

        // 所有圆一起跳
        for (int i = 0; i < circles.size(); i++)
        {
            if (circles[i].alive && circles[i].onGround)
            {
                circles[i].vy = -jumpSpeed;
                circles[i].onGround = false;
            }
        }

        // 所有方块一起跳
        for (int i = 0; i < squares.size(); i++)
        {
            if (squares[i].alive && squares[i].onGround)
            {
                squares[i].vy = -jumpSpeed;
                squares[i].onGround = false;
            }
        }
    }
}

void MyWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    if (event->key() == Qt::Key_Left)
        keyLeft = false;
    else if (event->key() == Qt::Key_Right)
        keyRight = false;
    else if (event->key() == Qt::Key_Up)
        keyUp = false;
}

void MyWindow::updateGame()
{
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

    // 圆和方块重叠：失败重开
    if (circlesHitSquares())
    {
        resetGame();
        update();
        return;
    }

    // 圆碰刺：失败重开
    for (int i = 0; i < circles.size(); i++)
    {
        if (circles[i].alive && roleHitSpike(circles[i]))
        {
            resetGame();
            update();
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
    doorOpen = allSquaresDead();

    // 任意活着的圆进入门：进入下一关
    if (doorOpen)
    {
        QRect doorRect(doorX, doorY, doorW, doorH);

        for (int i = 0; i < circles.size(); i++)
        {
            if (circles[i].alive && roleRect(circles[i]).intersects(doorRect))
            {
                timer->stop();

                if (currentLevel < 2)
                {
                    QMessageBox::information(this, "过关", QString("第 %1 关完成！").arg(currentLevel));
                }

                nextLevel();

                timer->start(16);
                update();
                return;
            }
        }
    }

    update();
}
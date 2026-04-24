#include "mywindow.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QDebug>

MyWindow::MyWindow(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(900, 600);
    setFocusPolicy(Qt::StrongFocus);

    bg.load(":/images/background.png");

    timer = new QTimer(this);

    keyLeft = false;
    keyRight = false;
    keyUp = false;

    initGame();

    connect(timer, &QTimer::timeout, this, &MyWindow::updateGame);
    timer->start(16);
}

MyWindow::~MyWindow()
{
}

void MyWindow::initGame()
{
    groundY = 524;

    moveSpeed = 5;
    jumpSpeed = 18;
    gravity = 1;

    // 圆
    circleW = 40;
    circleH = 40;
    circleX = 100;
    circleY = groundY - circleH;
    circleVx = 0;
    circleVy = 0;
    circleAlive = true;
    circleOnGround = true;

    // 方
    squareW = 40;
    squareH = 40;
    squareX = 220;
    squareY = groundY - squareH;
    squareVx = 0;
    squareVy = 0;
    squareAlive = true;
    squareOnGround = true;

    // 门
    doorOpen = false;
    doorW = 50;
    doorH = 80;
    doorX = 820;
    doorY = groundY - doorH;
}

void MyWindow::resetGame()
{
    keyLeft = false;
    keyRight = false;
    keyUp = false;

    circleVx = 0;
    circleVy = 0;
    squareVx = 0;
    squareVy = 0;

    initGame();
}

void MyWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 背景
    painter.drawPixmap(rect(), bg);

    // 地面
    // painter.setBrush(Qt::black);
    // painter.drawRect(0, groundY, width(), height() - groundY);

    // 左边墙
    // painter.drawRect(0, 0, 20, groundY);

    // 右边墙
    // painter.drawRect(width() - 20, 0, 20, groundY);

    // 刺（先画成几个三角形）
    QPolygon spike1;
    spike1 << QPoint(500, groundY)
           << QPoint(520, groundY - 30)
           << QPoint(540, groundY);

    QPolygon spike2;
    spike2 << QPoint(540, groundY)
           << QPoint(560, groundY - 30)
           << QPoint(580, groundY);

    painter.setBrush(Qt::red);
    painter.drawPolygon(spike1);
    painter.drawPolygon(spike2);

    // 圆
    if (circleAlive)
    {
        painter.setBrush(Qt::blue);
        painter.drawEllipse(circleX, circleY, circleW, circleH);
    }

    // 方块
    if (squareAlive)
    {
        painter.setBrush(Qt::darkGreen);
        painter.drawRect(squareX, squareY, squareW, squareH);
    }

    // 门
    // 门
    if (doorOpen)
    {
        painter.setBrush(QColor(139, 69, 19));
        painter.drawRect(doorX, doorY, doorW, doorH);

        painter.setBrush(Qt::yellow);
        painter.drawEllipse(doorX + doorW - 12, doorY + doorH / 2, 6, 6);
    }
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

        // 圆跳
        if (circleAlive && circleOnGround)
        {
            circleVy = -jumpSpeed;
            circleOnGround = false;
        }

        // 方跳
        if (squareAlive && squareOnGround)
        {
            squareVy = -jumpSpeed;
            squareOnGround = false;
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

bool MyWindow::objectHitSpike(int x, int y, int w, int h)
{
    // 两个刺三角形的三个顶点
    QPoint s1a(500, groundY);
    QPoint s1b(520, groundY - 30);
    QPoint s1c(540, groundY);

    QPoint s2a(540, groundY);
    QPoint s2b(560, groundY - 30);
    QPoint s2c(580, groundY);

    // 检查角色底边的三个点
    QPoint p1(x + 5,     y + h - 1);
    QPoint p2(x + w / 2, y + h - 1);
    QPoint p3(x + w - 5, y + h - 1);

    if (pointInTriangle(p1, s1a, s1b, s1c)) return true;
    if (pointInTriangle(p2, s1a, s1b, s1c)) return true;
    if (pointInTriangle(p3, s1a, s1b, s1c)) return true;

    if (pointInTriangle(p1, s2a, s2b, s2c)) return true;
    if (pointInTriangle(p2, s2a, s2b, s2c)) return true;
    if (pointInTriangle(p3, s2a, s2b, s2c)) return true;

    return false;
}

#include <QMessageBox>
#include <QRect>

void MyWindow::updateGame()
{
    int dir = 0;
    if (keyLeft && !keyRight)
        dir = -1;
    else if (keyRight && !keyLeft)
        dir = 1;

    // =========================
    // 1. 更新圆
    // =========================
    if (circleAlive)
    {
        circleVx = dir * moveSpeed;
        circleX += circleVx;

        // 左右边界
        if (circleX < 0)
            circleX = 0;
        if (circleX + circleW > width())
            circleX = width() - circleW;

        // 重力
        circleVy += gravity;
        circleY += circleVy;

        // 地面碰撞
        if (circleY + circleH >= groundY)
        {
            circleY = groundY - circleH;
            circleVy = 0;
            circleOnGround = true;
        }
        else
        {
            circleOnGround = false;
        }
    }

    // =========================
    // 2. 更新方块
    // =========================
    if (squareAlive)
    {
        squareVx = dir * moveSpeed;
        squareX += squareVx;

        // 左右边界
        if (squareX < 0)
            squareX = 0;
        if (squareX + squareW > width())
            squareX = width() - squareW;

        // 重力
        squareVy += gravity;
        squareY += squareVy;

        // 地面碰撞
        if (squareY + squareH >= groundY)
        {
            squareY = groundY - squareH;
            squareVy = 0;
            squareOnGround = true;
        }
        else
        {
            squareOnGround = false;
        }
    }

    // =========================
    // 3. 刺的碰撞区域
    QRect circleRect(circleX, circleY, circleW, circleH);
    QRect squareRect(squareX, squareY, squareW, squareH);

    // 圆碰刺：重开
    if (circleAlive && objectHitSpike(circleX, circleY, circleW, circleH))
    {
        resetGame();
        update();
        return;
    }

    // 方碰刺：死亡，开门
    if (squareAlive && objectHitSpike(squareX, squareY, squareW, squareH))
    {
        squareAlive = false;
        doorOpen = true;
    }

    // =========================
    // 4. 方块全死，门打开
    // 当前只有一个方块，所以方块死了就开门
    // =========================
    if (!squareAlive)
    {
        doorOpen = true;
    }

    // =========================
    // 5. 圆进入门：通关
    // =========================
    if (doorOpen && circleAlive)
    {
        QRect doorRect(doorX, doorY, doorW, doorH);

        if (circleRect.intersects(doorRect))
        {
            timer->stop();
            QMessageBox::information(this, "通关", "恭喜你通关！");
            resetGame();
            timer->start(16);
            update();
            return;   // 关键：这里必须直接结束
        }
    }

    update();
}
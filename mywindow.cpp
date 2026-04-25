#include "mywindow.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QDebug>
#include <QMessageBox>
#include <QPolygon>
#include "leveldata.h"

MyWindow::MyWindow(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(900, 600);
    setFocusPolicy(Qt::StrongFocus);

    bg.load(":/images/background.png");

    timer = new QTimer(this);


    resetTimer = new QTimer(this);
    resetTimer->setSingleShot(true);
    waitingReset = false;


    setupUI();
    initGame();

    connect(timer, &QTimer::timeout, this, &MyWindow::updateGame);
    timer->start(16);

    connect(resetTimer, &QTimer::timeout, this, [this]()
            {
                resetGame();
                waitingReset = false;

                if (sceneState == GameScene)
                {
                    timer->start(16);
                }

                update();
            });



    showMenu();
}

MyWindow::~MyWindow()
{
}

void MyWindow::scheduleReset()
{
    if (waitingReset)
        return;

    waitingReset = true;
    timer->stop();
    resetTimer->start(1000);   // 1秒后重开
}

void MyWindow::setupUI()
{
    btnLevel1 = new QPushButton("第1关", this);
    btnLevel2 = new QPushButton("第2关", this);
    btnLevel3 = new QPushButton("第3关", this);
    btnLevel4 = new QPushButton("第4关", this);
    btnBackToMenu = new QPushButton("返回主界面", this);

    btnLevel1->setGeometry(350, 180, 200, 50);
    btnLevel2->setGeometry(350, 250, 200, 50);
    btnLevel3->setGeometry(350, 320, 200, 50);
    btnLevel4->setGeometry(350, 390, 200, 50);

    btnBackToMenu->setGeometry(740, 20, 130, 40);

    connect(btnLevel1, &QPushButton::clicked, this, [=]() { startLevel(1); });
    connect(btnLevel2, &QPushButton::clicked, this, [=]() { startLevel(2); });
    connect(btnLevel3, &QPushButton::clicked, this, [=]() { startLevel(3); });
    connect(btnLevel4, &QPushButton::clicked, this, [=]() { startLevel(4); });

    connect(btnBackToMenu, &QPushButton::clicked, this, [=]() { showMenu(); });

    btnLevel1->setStyleSheet("font-size: 20px;");
    btnLevel2->setStyleSheet("font-size: 20px;");
    btnLevel3->setStyleSheet("font-size: 20px;");
    btnLevel4->setStyleSheet("font-size: 20px;");
    btnBackToMenu->setStyleSheet("font-size: 16px;");

    updateUIVisibility();
}

void MyWindow::updateUIVisibility()
{
    bool inMenu = (sceneState == MenuScene);
    bool inGame = (sceneState == GameScene);

    if (btnLevel1) btnLevel1->setVisible(inMenu);
    if (btnLevel2) btnLevel2->setVisible(inMenu);
    if (btnLevel3) btnLevel3->setVisible(inMenu);
    if (btnLevel4) btnLevel4->setVisible(inMenu);
    if (btnBackToMenu) btnBackToMenu->setVisible(inGame);
}

void MyWindow::showMenu()
{
    sceneState = MenuScene;

    keyLeft = false;
    keyRight = false;
    keyUp = false;

    if (resetTimer && resetTimer->isActive())
        resetTimer->stop();

    waitingReset = false;

    if (timer)
        timer->stop();

    updateUIVisibility();
    update();
}

void MyWindow::startLevel(int level)
{
    currentLevel = level;
    loadLevel(currentLevel);

    sceneState = GameScene;

    keyLeft = false;
    keyRight = false;
    keyUp = false;

    updateUIVisibility();
    timer->start(16);
    update();
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

    sceneState = MenuScene;
}


void MyWindow::resetGame()
{
    keyLeft = false;
    keyRight = false;
    keyUp = false;

    loadLevel(currentLevel);
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


void MyWindow::loadLevel(int level)
{
    keyLeft = false;
    keyRight = false;
    keyUp = false;

    LevelData data = buildLevelData(level, width());

    circles = data.circles;
    squares = data.squares;
    spikes = data.spikes;
    blocks = data.blocks;

    groundY = data.groundY;

    doorW = data.doorW;
    doorH = data.doorH;
    doorX = data.doorX;
    doorY = data.doorY;

    doorOpen = false;
}


void MyWindow::nextLevel()
{
    currentLevel++;

    if (currentLevel > 4)
    {
        QMessageBox::information(this, "游戏完成", "恭喜你完成了全部关卡！");
        currentLevel = 1;
        showMenu();
        return;
    }

    loadLevel(currentLevel);
}


QRect MyWindow::roleRect(const Role &role) const
{
    return QRect(role.x, role.y, role.w, role.h);
}

QRect MyWindow::blockRect(const Block &block) const
{
    return QRect(block.x, block.y, block.w, block.h);
}

void MyWindow::drawRole(QPainter &painter, const Role &role)
{
    if (!role.alive || role.escaped)
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
    if (!role.alive || role.escaped)
        return;

    // =========================
    // 1. 水平移动
    // =========================
    role.vx = dir * moveSpeed;

    int oldX = role.x;
    role.x += role.vx;

    // 左右边界
    if (role.x < 0)
        role.x = 0;
    if (role.x + role.w > width())
        role.x = width() - role.w;

    // 和平台的左右碰撞
    QRect r = roleRect(role);
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

    // =========================
    // 2. 垂直移动
    // =========================
    int oldY = role.y;

    role.vy += gravity;
    role.y += role.vy;
    role.onGround = false;

    r = roleRect(role);

    // 先处理和平台的上下碰撞
    for (int i = 0; i < blocks.size(); i++)
    {
        QRect b = blockRect(blocks[i]);

        if (r.intersects(b))
        {
            // 从上方落到平台上
            if (oldY + role.h <= blocks[i].y)
            {
                role.y = blocks[i].y - role.h;
                role.vy = 0;
                role.onGround = true;
            }
            // 从下方顶到平台底部
            else if (oldY >= blocks[i].y + blocks[i].h)
            {
                role.y = blocks[i].y + blocks[i].h;
                role.vy = 0;
            }

            r = roleRect(role);
        }
    }

    // 再处理最底部地面
    if (role.y + role.h >= groundY)
    {
        role.y = groundY - role.h;
        role.vy = 0;
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

void MyWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (sceneState == MenuScene)
    {
        painter.drawPixmap(rect(), bg);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 28, QFont::Bold));
        painter.drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, "Shape Puzzle");

        painter.setFont(QFont("Arial", 16));
        painter.drawText(0, 100, width(), 40, Qt::AlignHCenter, "请选择关卡");

        return;
    }

    // 背景
    painter.drawPixmap(rect(), bg);

    // block
    painter.setBrush(Qt::black);
    for (int i = 0; i < blocks.size(); i++)
    {
        painter.drawRect(blocks[i].x, blocks[i].y, blocks[i].w, blocks[i].h);
    }

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

    if (waitingReset)
    {
        painter.fillRect(rect(), QColor(0, 0, 0, 100));
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

bool MyWindow::allCirclesEscaped() const
{
    for (int i = 0; i < circles.size(); i++)
    {
        if (circles[i].alive && !circles[i].escaped)
            return false;
    }
    return true;
}


void MyWindow::updateGame()
{
    if (sceneState != GameScene || waitingReset)
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
    doorOpen = allSquaresDead();

    // all活着的圆进入门：进入下一关
    if (doorOpen)
    {
        QRect doorRect(doorX, doorY, doorW, doorH);

        // 先检查有没有圆进入门
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

        // 所有圆都进门后，才真正通关
        if (allCirclesEscaped())
        {
            timer->stop();

            if (currentLevel < 4)
            {
                QMessageBox::information(this, "过关", QString("第 %1 关完成！").arg(currentLevel));
            }

            nextLevel();

            timer->start(16);
            update();
            return;
        }
    }

    update();
}
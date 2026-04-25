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

    totalLevels = 6;
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



void MyWindow::setupUI()
{
    levelButtons.clear();

    int btnW = 140;
    int btnH = 50;
    int startX = 180;
    int startY = 180;
    int gapX = 160;
    int gapY = 80;
    int columns = 3;

    for (int i = 0; i < totalLevels; i++)
    {
        QPushButton *btn = new QPushButton(QString("第%1关").arg(i + 1), this);

        int row = i / columns;
        int col = i % columns;

        int x = startX + col * gapX;
        int y = startY + row * gapY;

        btn->setGeometry(x, y, btnW, btnH);
        btn->setStyleSheet("font-size: 18px;");

        connect(btn, &QPushButton::clicked, this, [=]()
                {
                    startLevel(i + 1);
                });

        levelButtons.append(btn);
    }

    btnBackToMenu = new QPushButton("返回主界面", this);
    btnBackToMenu->setGeometry(730, 20, 140, 40);
    btnBackToMenu->setStyleSheet("font-size: 15px;");

    connect(btnBackToMenu, &QPushButton::clicked, this, [=]()
            {
                showMenu();
            });

    updateUIVisibility();
}

void MyWindow::updateUIVisibility()
{
    bool inMenu = (sceneState == MenuScene);
    bool inGame = (sceneState == GameScene);

    for (int i = 0; i < levelButtons.size(); i++)
    {
        if (levelButtons[i])
            levelButtons[i]->setVisible(inMenu);
    }

    if (btnBackToMenu)
        btnBackToMenu->setVisible(inGame);
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
    movingSpikes = data.movingSpikes;

    groundY = data.groundY;

    doorW = data.doorW;
    doorH = data.doorH;
    doorX = data.doorX;
    doorY = data.doorY;

    doorOpen = data.doorOpenAtStart;

    movingSpikesStarted = false;
    movingSpikeDelayFrames = 0;
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

    // 画移动刺
    painter.setBrush(QColor(255, 140, 0));   // 橙色，和静态红刺区分
    for (int i = 0; i < movingSpikes.size(); i++)
    {
        for (int j = 0; j < movingSpikes[i].count; j++)
        {
            int sx = movingSpikes[i].x + j * 20;
            int sy = movingSpikes[i].y;
            int h  = movingSpikes[i].height;

            QPolygon spikePoly;
            spikePoly << QPoint(sx, sy)
                      << QPoint(sx + 10, sy - h)
                      << QPoint(sx + 20, sy);

            painter.drawPolygon(spikePoly);
        }
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




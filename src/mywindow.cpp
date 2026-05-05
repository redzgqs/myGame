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
    realImg.load(":/images/real.png");
    fakeImg.load(":/images/fake.png");
    menuLeftImg.load(":/images/menu_left.png");
    menuRightImg.load(":/images/menu_right.png");
    doorClosedImg.load(":/images/door_closed.png");
    doorOpenImg.load(":/images/door_open.png");
    spikeStaticImg.load(":/images/spike_static.png");
    spikeMovingImg.load(":/images/spike_moving.png");
    finishNailongImg.load(":/images/finish_nailong.png");
    failNailongImg.load(":/images/fail_nailong.png");

    clearTimer = new QTimer(this);
    clearTimer->setSingleShot(true);
    waitingNextLevel = false;

    levelClearImg.load(":/images/level_clear.png");

    connect(clearTimer, &QTimer::timeout, this, [this]()
            {
                waitingNextLevel = false;
                nextLevel();

                if (sceneState == GameScene)
                    timer->start(16);

                update();
            });

    timer = new QTimer(this);


    resetTimer = new QTimer(this);
    resetTimer->setSingleShot(true);
    waitingReset = false;

    totalLevels = 9;
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

    int btnW = 70;
    int btnH = 50;
    int startX = 330;
    int startY = 230;
    int gapX = 90;
    int gapY = 70;
    int columns = 3;

    for (int i = 0; i < totalLevels; i++)
    {
        QPushButton *btn = new QPushButton(QString::number(i + 1), this);

        int row = i / columns;
        int col = i % columns;

        int x = startX + col * gapX;
        int y = startY + row * gapY;

        btn->setGeometry(x, y, btnW, btnH);
        btn->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "border-radius: 10px;"
            );

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
    bool inFinish = (sceneState == FinishScene);

    for (int i = 0; i < levelButtons.size(); i++)
    {
        if (levelButtons[i])
            levelButtons[i]->setVisible(inMenu);
    }

    if (btnBackToMenu)
        btnBackToMenu->setVisible(inGame || inFinish);
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

    if (clearTimer && clearTimer->isActive())
        clearTimer->stop();

    waitingNextLevel = false;

    if (timer)
        timer->stop();

    if (btnBackToMenu)
    {
        btnBackToMenu->setText("返回主界面");
        btnBackToMenu->setGeometry(730, 20, 140, 40);
    }

    updateUIVisibility();
    update();
}

void MyWindow::startLevel(int level)
{
    waitingNextLevel = false;
    currentLevel = level;
    loadLevel(currentLevel);

    sceneState = GameScene;

    keyLeft = false;
    keyRight = false;
    keyUp = false;

    if (btnBackToMenu)
    {
        btnBackToMenu->setText("返回主界面");
        btnBackToMenu->setGeometry(730, 20, 140, 40);
    }

    updateUIVisibility();
    timer->start(16);
    update();
}

void MyWindow::initGame()
{
    keyLeft = false;
    keyRight = false;
    keyUp = false;
    jumpBufferFrames = 0;

    moveSpeed = 4;
    jumpSpeed = 16;
    gravity = 0.65;

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
    jumpBufferFrames = 0;

    LevelData data = buildLevelData(level, width());

    circles = data.circles;
    squares = data.squares;
    spikes = data.spikes;
    blocks = data.blocks;
    movingSpikes = data.movingSpikes;
    hiddenSpikes = data.hiddenSpikes;

    groundY = data.groundY;

    doorW = data.doorW;
    doorH = data.doorH;
    doorX = data.doorX;
    doorY = data.doorY;

    doorOpenAtStart = data.doorOpenAtStart;
    doorOpen = doorOpenAtStart;

    movingSpikesStarted = false;
    movingSpikeDelayFrames = 0;

    waitingNextLevel = false;
}



void MyWindow::drawRole(QPainter &painter, const Role &role)
{
    if (!role.alive || role.escaped)
        return;

    QRect target(role.x, role.y, role.w, role.h);

    if (role.isCircle)
    {
        painter.drawPixmap(target, realImg);
    }
    else
    {
        painter.drawPixmap(target, fakeImg);
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

        // 左侧图片
        if (!menuLeftImg.isNull())
        {
            QRect leftRect(60, 259, 210, 270);
            painter.drawPixmap(leftRect, menuLeftImg);
        }

        // 右侧图片
        if (!menuRightImg.isNull())
        {
            QRect rightRect(660, 215, 180, 320);
            painter.drawPixmap(rightRect, menuRightImg);
        }

        painter.setPen(Qt::white);
        painter.setFont(QFont("幼圆", 28, QFont::Bold));
        painter.drawText(0, 50, width(), 50, Qt::AlignHCenter, "我才是奶龙");

        painter.setFont(QFont("幼圆", 16));
        painter.drawText(0, 115, width(), 40, Qt::AlignHCenter, "关卡");

        return;
    }
    if (sceneState == FinishScene)
    {
        painter.drawPixmap(rect(), bg);

        // 稍微加一层暗色遮罩
        painter.fillRect(rect(), QColor(0, 0, 0, 90));

        // 画奶龙图片
        if (!finishNailongImg.isNull())
        {
            QRect imgRect(300, 110, 280, 300);
            painter.drawPixmap(imgRect, finishNailongImg);
        }

        // 文案
        painter.setPen(Qt::white);
        painter.setFont(QFont("幼圆", 28, QFont::Bold));
        painter.drawText(0, 40, width(), 50, Qt::AlignHCenter, "谢谢你");

        painter.setFont(QFont("幼圆", 22, QFont::Bold));
        painter.drawText(0, 420, width(), 40, Qt::AlignHCenter, "你拯救了奶龙！");

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
    for (int i = 0; i < spikes.size(); i++)
    {
        int left   = spikes[i].a.x();
        int top    = spikes[i].b.y();
        int width  = spikes[i].c.x() - spikes[i].a.x();
        int height = spikes[i].a.y() - spikes[i].b.y();

        QRect target(left, top-4, width, height+4);

        if (!spikeStaticImg.isNull())
        {
            painter.drawPixmap(target, spikeStaticImg);
        }
        else
        {
            // 图片没加载成功时，仍然退回到原来的三角形绘制
            painter.setBrush(Qt::red);
            QPolygon spikePoly;
            spikePoly << spikes[i].a << spikes[i].b << spikes[i].c;
            painter.drawPolygon(spikePoly);
        }
    }

    // 门
    QRect doorRect(doorX, doorY, doorW, doorH);

    if (doorOpen)
    {
        if (!doorOpenImg.isNull())
            painter.drawPixmap(doorRect, doorOpenImg);
        else
            painter.drawRect(doorRect);
    }
    else
    {
        if (!doorClosedImg.isNull())
            painter.drawPixmap(doorRect, doorClosedImg);
        else
            painter.drawRect(doorRect);
    }

    // 画移动刺
    for (int i = 0; i < movingSpikes.size(); i++)
    {
        for (int j = 0; j < movingSpikes[i].count; j++)
        {
            int sx = movingSpikes[i].x + j * 20;
            int sy = movingSpikes[i].y;
            int h  = movingSpikes[i].height;

            QRect target(sx, sy - h, 20, h);

            if (!spikeMovingImg.isNull())
            {
                painter.drawPixmap(target, spikeMovingImg);
            }
            else
            {
                painter.setBrush(QColor(255, 140, 0));
                QPolygon spikePoly;
                spikePoly << QPoint(sx, sy)
                          << QPoint(sx + 10, sy - h)
                          << QPoint(sx + 20, sy);

                painter.drawPolygon(spikePoly);
            }
        }
    }

    // 画隐藏刺（只有触发后才显示）
    for (int i = 0; i < hiddenSpikes.size(); i++)
    {
        if (!hiddenSpikes[i].visible)
            continue;

        int left   = hiddenSpikes[i].a.x();
        int top    = hiddenSpikes[i].b.y();
        int width  = hiddenSpikes[i].c.x() - hiddenSpikes[i].a.x();
        int height = hiddenSpikes[i].a.y() - hiddenSpikes[i].b.y();

        QRect target(left, top, width, height);

        if (!spikeStaticImg.isNull())
        {
            painter.drawPixmap(target, spikeStaticImg);
        }
        else
        {
            QPolygon spikePoly;
            spikePoly << hiddenSpikes[i].a << hiddenSpikes[i].b << hiddenSpikes[i].c;
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



    // 关卡
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(20, 30, QString("Level %1").arg(currentLevel));



    if (waitingReset)
    {
        // 先变暗
        painter.fillRect(rect(), QColor(0, 0, 0, 120));

        // 再画失败奶龙图
        if (!failNailongImg.isNull())
        {
            QRect failImgRect(290, 120, 320, 360);
            painter.drawPixmap(failImgRect, failNailongImg);
        }

        // 再加一句字
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(0, 500, width(), 40, Qt::AlignHCenter, "哈哈哈哈");
    }

    if (waitingNextLevel)
    {
        painter.fillRect(rect(), QColor(0, 0, 0, 120));

        if (!levelClearImg.isNull())
        {
            QRect clearImgRect(300, 120, 280, 300);
            painter.drawPixmap(clearImgRect, levelClearImg);
        }

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(0, 455, width(), 40, Qt::AlignHCenter, "牛福");
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
        jumpBufferFrames = 8;   // 约 8 帧缓冲
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


void MyWindow::showFinishScene()
{
    sceneState = FinishScene;

    keyLeft = false;
    keyRight = false;
    keyUp = false;

    if (timer)
        timer->stop();

    if (resetTimer && resetTimer->isActive())
        resetTimer->stop();

    waitingReset = false;

    if (btnBackToMenu)
    {
        btnBackToMenu->setText("返回主界面");
        btnBackToMenu->setGeometry(360, 500, 180, 45);
    }

    updateUIVisibility();
    update();
}



#include "mywindow.h"

#include <QBrush>
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <cmath>

#include "leveldata.h"

MyWindow::MyWindow(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(900, 600);
    setFocusPolicy(Qt::StrongFocus);

    bg.load(":/images/background.png");
    rulesImg.load(":/images/rules.png");
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
    levelClearImg.load(":/images/level_clear.png");

    failSound = new QSoundEffect(this);
    failSound->setSource(QUrl::fromLocalFile("D:/MyCodeQt/prj/I_am_nailong/audio/fail.wav"));
    failSound->setLoopCount(1);
    failSound->setVolume(0.8);

    bgmPlayer = new QMediaPlayer(this);
    bgmAudioOutput = new QAudioOutput(this);

    bgmPlayer->setAudioOutput(bgmAudioOutput);
    bgmAudioOutput->setVolume(0.35);
    bgmPlayer->setSource(QUrl::fromLocalFile("D:/MyCodeQt/prj/I_am_nailong/audio/bgm.mp3"));
    bgmPlayer->setLoops(QMediaPlayer::Infinite);
    bgmPlayer->play();

    timer = new QTimer(this);

    resetTimer = new QTimer(this);
    resetTimer->setSingleShot(true);
    waitingReset = false;

    clearTimer = new QTimer(this);
    clearTimer->setSingleShot(true);
    waitingNextLevel = false;

    connect(timer, &QTimer::timeout, this, &MyWindow::updateGame);

    connect(resetTimer, &QTimer::timeout, this, [this]() {
        resetGame();
        waitingReset = false;

        if (sceneState == GameScene)
        {
            timer->start(16);
        }

        update();
    });

    connect(clearTimer, &QTimer::timeout, this, [this]() {
        waitingNextLevel = false;
        nextLevel();

        if (sceneState == GameScene)
        {
            timer->start(16);
        }

        update();
    });

    fadeAlpha = 255;
    fadeDirection = -1;
    isFading = true;

    failPopupScale = 0.8;
    clearPopupScale = 0.8;

    totalLevels = 9;

    setupUI();
    initGame();
    timer->start(16);
    showMenu();


}

MyWindow::~MyWindow()
{
}

void MyWindow::setupUI()
{
    levelButtons.clear();

    const int btnW = 70;
    const int btnH = 50;
    const int startX = 330;
    const int startY = 230;
    const int gapX = 90;
    const int gapY = 70;
    const int columns = 3;

    for (int i = 0; i < totalLevels; i++)
    {
        QPushButton *btn = new QPushButton(QString::number(i + 1), this);

        const int row = i / columns;
        const int col = i % columns;
        const int x = startX + col * gapX;
        const int y = startY + row * gapY;

        btn->setGeometry(x, y, btnW, btnH);
        btn->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "border-radius: 10px;"
            );

        connect(btn, &QPushButton::clicked, this, [=]() {
            startLevel(i + 1);
        });

        levelButtons.append(btn);
    }

    btnRules = new QPushButton("查看规则", this);
    btnRules->setGeometry(380, 420, 140, 45);
    btnRules->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
        "border-radius: 10px;"
        );

    connect(btnRules, &QPushButton::clicked, this, [=]()
            {
                showRulesScene();
            });

    btnBackToMenu = new QPushButton("返回主界面", this);
    btnBackToMenu->setGeometry(730, 20, 140, 40);
    btnBackToMenu->setStyleSheet("font-size: 15px;");

    connect(btnBackToMenu, &QPushButton::clicked, this, [=]() {
        showMenu();
    });

    updateUIVisibility();
}

void MyWindow::updateUIVisibility()
{
    bool inMenu = (sceneState == MenuScene);
    bool inGame = (sceneState == GameScene);
    bool inRules = (sceneState == RulesScene);
    bool inFinish = (sceneState == FinishScene);

    for (int i = 0; i < levelButtons.size(); i++)
    {
        if (levelButtons[i])
            levelButtons[i]->setVisible(inMenu);
    }

    if (btnRules)
        btnRules->setVisible(inMenu);

    if (btnBackToMenu)
        btnBackToMenu->setVisible(inGame || inRules || inFinish);
}

void MyWindow::showMenu()
{
    sceneState = MenuScene;

    fadeAlpha = 255;
    fadeDirection = -1;
    isFading = true;

    keyLeft = false;
    keyRight = false;
    keyUp = false;

    if (resetTimer && resetTimer->isActive())
        resetTimer->stop();

    if (clearTimer && clearTimer->isActive())
        clearTimer->stop();

    waitingReset = false;
    waitingNextLevel = false;

    if (timer && !timer->isActive())
        timer->start(16);

    if (btnBackToMenu)
    {
        btnBackToMenu->setText("返回主界面");
        btnBackToMenu->setGeometry(730, 20, 140, 40);
    }

    updateUIVisibility();
    update();
}

void MyWindow::showRulesScene()
{
    sceneState = RulesScene;

    fadeAlpha = 255;
    fadeDirection = -1;
    isFading = true;

    keyLeft = false;
    keyRight = false;
    keyUp = false;

    if (resetTimer && resetTimer->isActive())
        resetTimer->stop();

    if (clearTimer && clearTimer->isActive())
        clearTimer->stop();

    waitingReset = false;
    waitingNextLevel = false;

    if (btnBackToMenu)
    {
        btnBackToMenu->setText("返回");
        btnBackToMenu->setGeometry(380, 540, 140, 40);
    }

    if (timer && !timer->isActive())
        timer->start(16);

    updateUIVisibility();
    update();
}

void MyWindow::showFinishScene()
{
    sceneState = FinishScene;

    fadeAlpha = 255;
    fadeDirection = -1;
    isFading = true;

    keyLeft = false;
    keyRight = false;
    keyUp = false;

    if (resetTimer && resetTimer->isActive())
        resetTimer->stop();

    waitingReset = false;

    if (btnBackToMenu)
    {
        btnBackToMenu->setText("返回主界面");
        btnBackToMenu->setGeometry(360, 500, 180, 45);
    }

    if (timer && !timer->isActive())
        timer->start(16);

    updateUIVisibility();
    update();
}

void MyWindow::startLevel(int level)
{
    waitingNextLevel = false;
    currentLevel = level;
    loadLevel(currentLevel);

    sceneState = GameScene;

    fadeAlpha = 255;
    fadeDirection = -1;
    isFading = true;

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
    animFrame = 0;

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
    animFrame = 0;

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

    const QPixmap &pix = role.isCircle ? realImg : fakeImg;
    if (pix.isNull())
        return;

    double drawX = role.x;
    double drawY = role.y;
    double drawW = role.w;
    double drawH = role.h;


    if (role.onGround && std::abs(role.vx) < 0.01)
    {
        double floatOffset = std::sin(animFrame * 0.10 + role.x * 0.03) * 2.5;
        drawY += floatOffset;
    }


    if (!role.onGround)
    {
        if (role.vy < -0.1)
        {
            drawW = role.w - 5;
            drawH = role.h + 5;
            drawX += 3;
            drawY -= 6;
        }
        else if (role.vy > 0.1)
        {
            drawW = role.w - 4;
            drawH = role.h + 4;
            drawX += 2;
            drawY -= 4;
        }
    }


    if (role.landAnimFrames > 0)
    {
        double t = role.landAnimFrames / 8.0;
        double extraW = 10.0 * t;
        double lessH = 8.0 * t;

        drawX -= extraW / 2.0;
        drawY += lessH;
        drawW += extraW;
        drawH -= lessH;
    }

    int finalW = (int)std::round(drawW);
    int finalH = (int)std::round(drawH);

    if (finalW <= 0 || finalH <= 0)
        return;

    QRect target(
        (int)std::round(drawX),
        (int)std::round(drawY),
        finalW,
        finalH
        );


    if (role.faceRight)
    {
        painter.drawPixmap(target, pix);
    }
    else
    {
        painter.save();
        painter.translate(target.x() + target.width(), target.y());
        painter.scale(-1, 1);
        painter.drawPixmap(0, 0, target.width(), target.height(), pix);
        painter.restore();
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

        if (!menuLeftImg.isNull())
        {
            QRect leftRect(60, 259, 210, 270);
            painter.drawPixmap(leftRect, menuLeftImg);
        }

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

        if (fadeAlpha > 0)
        {
            painter.fillRect(rect(), QColor(0, 0, 0, fadeAlpha));
        }

        return;
    }

    if (sceneState == RulesScene)
    {
        painter.drawPixmap(rect(), bg);
        painter.fillRect(rect(), QColor(0, 0, 0, 40));

        if (!rulesImg.isNull())
        {
            QRect rulesRect(0, 0, width(), height()- 70);
            painter.drawPixmap(rulesRect, rulesImg);
        }

        if (fadeAlpha > 0)
        {
            painter.fillRect(rect(), QColor(0, 0, 0, fadeAlpha));
        }

        return;
    }

    if (sceneState == FinishScene)
    {
        painter.drawPixmap(rect(), bg);
        painter.fillRect(rect(), QColor(0, 0, 0, 90));

        if (!finishNailongImg.isNull())
        {
            QRect imgRect(300, 110, 280, 300);
            painter.drawPixmap(imgRect, finishNailongImg);
        }

        painter.setPen(Qt::white);
        painter.setFont(QFont("幼圆", 28, QFont::Bold));
        painter.drawText(0, 40, width(), 50, Qt::AlignHCenter, "谢谢你");

        painter.setFont(QFont("幼圆", 22, QFont::Bold));
        painter.drawText(0, 420, width(), 40, Qt::AlignHCenter, "你拯救了奶龙！");

        if (fadeAlpha > 0)
        {
            painter.fillRect(rect(), QColor(0, 0, 0, fadeAlpha));
        }

        return;
    }

    painter.drawPixmap(rect(), bg);

    painter.setBrush(Qt::black);
    for (int i = 0; i < blocks.size(); i++)
    {
        painter.drawRect(blocks[i].x, blocks[i].y, blocks[i].w, blocks[i].h);
    }

    for (int i = 0; i < spikes.size(); i++)
    {
        const int left = spikes[i].a.x();
        const int top = spikes[i].b.y();
        const int width = spikes[i].c.x() - spikes[i].a.x();
        const int height = spikes[i].a.y() - spikes[i].b.y();
        QRect target(left, top - 4, width, height + 4);

        if (!spikeStaticImg.isNull())
        {
            painter.drawPixmap(target, spikeStaticImg);
        }
        else
        {
            painter.setBrush(Qt::red);
            QPolygon spikePoly;
            spikePoly << spikes[i].a << spikes[i].b << spikes[i].c;
            painter.drawPolygon(spikePoly);
        }
    }

    QRect doorRect(doorX, doorY, doorW, doorH);
    if (doorOpen)
    {
        if (!doorOpenImg.isNull())
        {
            painter.drawPixmap(doorRect, doorOpenImg);
        }
        else
        {
            painter.drawRect(doorRect);
        }
    }
    else
    {
        if (!doorClosedImg.isNull())
        {
            painter.drawPixmap(doorRect, doorClosedImg);
        }
        else
        {
            painter.drawRect(doorRect);
        }
    }

    for (int i = 0; i < movingSpikes.size(); i++)
    {
        for (int j = 0; j < movingSpikes[i].count; j++)
        {
            const int sx = movingSpikes[i].x + j * 20;
            const int sy = movingSpikes[i].y;
            const int h = movingSpikes[i].height;
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

    for (int i = 0; i < hiddenSpikes.size(); i++)
    {
        if (!hiddenSpikes[i].visible)
        {
            continue;
        }

        const int left = hiddenSpikes[i].a.x();
        const int top = hiddenSpikes[i].b.y();
        const int width = hiddenSpikes[i].c.x() - hiddenSpikes[i].a.x();
        const int height = hiddenSpikes[i].a.y() - hiddenSpikes[i].b.y();
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

    for (int i = 0; i < circles.size(); i++)
    {
        drawRole(painter, circles[i]);
    }

    for (int i = 0; i < squares.size(); i++)
    {
        drawRole(painter, squares[i]);
    }

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(20, 30, QString("Level %1").arg(currentLevel));

    if (waitingReset)
    {
        painter.fillRect(rect(), QColor(0, 0, 0, 160));

        if (!failNailongImg.isNull())
        {
            int baseW = 320;
            int baseH = 360;

            int drawW = (int)(baseW * failPopupScale);
            int drawH = (int)(baseH * failPopupScale);

            int drawX = (width() - drawW) / 2;
            int drawY = 120 + (baseH - drawH) / 2;

            QRect failImgRect(drawX, drawY, drawW, drawH);
            painter.drawPixmap(failImgRect, failNailongImg);
        }

        painter.setPen(Qt::white);
        painter.setFont(QFont("幼圆", 24, QFont::Bold));
        painter.drawText(0, 500, width(), 40, Qt::AlignHCenter, "哈哈哈哈");
    }

    if (waitingNextLevel)
    {
        painter.fillRect(rect(), QColor(0, 0, 0, 160));

        if (!levelClearImg.isNull())
        {
            int baseW = 280;
            int baseH = 300;

            int drawW = (int)(baseW * clearPopupScale);
            int drawH = (int)(baseH * clearPopupScale);

            int drawX = (width() - drawW) / 2;
            int drawY = 120 + (baseH - drawH) / 2;

            QRect clearImgRect(drawX, drawY, drawW, drawH);
            painter.drawPixmap(clearImgRect, levelClearImg);
        }

        painter.setPen(Qt::white);
        painter.setFont(QFont("幼圆", 24, QFont::Bold));
        painter.drawText(0, 455, width(), 40, Qt::AlignHCenter, "牛福");
    }

    if (fadeAlpha > 0)
    {
        painter.fillRect(rect(), QColor(0, 0, 0, fadeAlpha));
    }
}

void MyWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        return;
    }

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
        jumpBufferFrames = 8;
    }
}

void MyWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        return;
    }

    if (event->key() == Qt::Key_Left)
    {
        keyLeft = false;
    }
    else if (event->key() == Qt::Key_Right)
    {
        keyRight = false;
    }
    else if (event->key() == Qt::Key_Up)
    {
        keyUp = false;
    }
}

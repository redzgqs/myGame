#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QKeyEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QPoint>
#include <QPushButton>
#include <QRect>
#include <QTimer>
#include <QVector>
#include <QWidget>
#include "gameobjects.h"

class QPainter;

enum SceneState
{
    MenuScene,
    GameScene,
    RulesScene,
    FinishScene
};

class MyWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MyWindow(QWidget *parent = nullptr);
    ~MyWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updateGame();

private:
    // 初始化与关卡切换
    void initGame();
    void resetGame();
    void loadLevel(int level);
    void nextLevel();

    // 场景与界面
    void setupUI();
    void updateUIVisibility();
    void showMenu();
    void showFinishScene();
    void showRulesScene();
    void startLevel(int level);

    // 绘制
    void drawRole(QPainter &painter, const Role &role);

    // 角色与机关更新
    void updateRole(Role &role, int dir);
    void updateMovingSpikes();
    void updateHiddenSpikes();

    // 碰撞与判定
    QRect roleRect(const Role &role) const;
    QRect blockRect(const Block &block) const;
    bool pointInTriangle(QPoint p, QPoint a, QPoint b, QPoint c);
    bool roleHitSpike(const Role &role);
    bool circlesHitSquares() const;
    bool allSquaresDead() const;
    bool allCirclesEscaped() const;

    // 失败与过场
    void scheduleReset();
    void scheduleNextLevel();
    void killCircleAndReset(int index);

private:
    QTimer *timer;
    QTimer *resetTimer;
    QTimer *clearTimer;

    // 场景状态
    SceneState sceneState;
    bool waitingReset;
    bool waitingNextLevel;

    // 图片资源
    QPixmap bg;
    QPixmap rulesImg;
    QPixmap realImg;
    QPixmap fakeImg;
    QPixmap menuLeftImg;
    QPixmap menuRightImg;
    QPixmap doorClosedImg;
    QPixmap doorOpenImg;
    QPixmap spikeStaticImg;
    QPixmap spikeMovingImg;
    QPixmap finishNailongImg;
    QPixmap failNailongImg;
    QPixmap levelClearImg;

    // 输入状态
    bool keyLeft;
    bool keyRight;
    bool keyUp;
    int jumpBufferFrames;

    // 游戏对象
    QVector<Role> circles;
    QVector<Role> squares;
    QVector<Spike> spikes;
    QVector<HiddenSpike> hiddenSpikes;
    QVector<Block> blocks;
    QVector<MovingSpike> movingSpikes;

    // 移动刺控制
    bool movingSpikesStarted;
    int movingSpikeDelayFrames;

    // 关卡与地图参数
    int currentLevel;
    int totalLevels;
    int groundY;

    // 门
    bool doorOpen;
    bool doorOpenAtStart;
    int doorX;
    int doorY;
    int doorW;
    int doorH;

    // 物理参数
    int moveSpeed;
    int jumpSpeed;
    double gravity;

    // 菜单按钮
    QVector<QPushButton *> levelButtons;
    QPushButton *btnRules;
    QPushButton *btnBackToMenu;
};

#endif // MYWINDOW_H

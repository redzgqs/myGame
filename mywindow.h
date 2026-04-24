#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QVector>
#include <QPoint>
#include <QRect>
#include "gameobjects.h"


class MyWindow : public QWidget
{
    Q_OBJECT

public:
    MyWindow(QWidget *parent = nullptr);
    ~MyWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updateGame();

private:
    void initGame();
    void resetGame();

    void loadLevel(int level);
    void nextLevel();

    void drawRole(QPainter &painter, const Role &role);
    void updateRole(Role &role, int dir);
    QRect roleRect(const Role &role) const;

    bool pointInTriangle(QPoint p, QPoint a, QPoint b, QPoint c);
    bool roleHitSpike(const Role &role);
    bool allSquaresDead() const;
    bool circlesHitSquares() const;

    QRect blockRect(const Block &block) const;

    bool allCirclesEscaped() const;

private:
    QTimer *timer;
    QPixmap bg;

    // 按键状态
    bool keyLeft;
    bool keyRight;
    bool keyUp;

    // 多个圆和多个方块
    QVector<Role> circles;
    QVector<Role> squares;

    // 所有刺
    QVector<Spike> spikes;

    // block
    QVector<Block> blocks;

    // 当前关卡
    int currentLevel;

    // 地图参数
    int groundY;

    // 门
    bool doorOpen;
    int doorX;
    int doorY;
    int doorW;
    int doorH;

    // 物理参数
    int moveSpeed;
    int jumpSpeed;
    int gravity;
};

#endif // MYWINDOW_H
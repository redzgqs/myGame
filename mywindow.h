#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QPixmap>
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QPaintEvent>

class MyWindow : public QWidget
{
    Q_OBJECT


public:
    MyWindow(QWidget *parent = nullptr);
    ~MyWindow();

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void updateGame();

private:
    void initGame();
    void resetGame();
    bool pointInTriangle(QPoint p, QPoint a, QPoint b, QPoint c);
    bool objectHitSpike(int x, int y, int w, int h);

private:
    QTimer *timer;
    QPixmap bg;

    // 按键状态
    bool keyLeft;
    bool keyRight;
    bool keyUp;

    // 圆
    int circleX;
    int circleY;
    int circleW;
    int circleH;
    int circleVx;
    int circleVy;
    bool circleAlive;
    bool circleOnGround;

    // 方块
    int squareX;
    int squareY;
    int squareW;
    int squareH;
    int squareVx;
    int squareVy;
    bool squareAlive;
    bool squareOnGround;

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
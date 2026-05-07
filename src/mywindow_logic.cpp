#include "mywindow.h"

void MyWindow::scheduleReset()
{
    if (waitingReset)
    {
        return;
    }

    waitingReset = true;
    timer->stop();
    resetTimer->start(1000);
}

void MyWindow::scheduleNextLevel()
{
    if (waitingNextLevel)
    {
        return;
    }

    waitingNextLevel = true;
    timer->stop();
    update();
    clearTimer->start(1000);
}

void MyWindow::killCircleAndReset(int index)
{
    if (index < 0 || index >= circles.size())
    {
        return;
    }

    circles[index].alive = false;
    circles[index].vx = 0;
    circles[index].vy = 0;

    update();
    scheduleReset();
}

void MyWindow::nextLevel()
{
    currentLevel++;

    if (currentLevel > 9)
    {
        showFinishScene();
        return;
    }

    loadLevel(currentLevel);
}

void MyWindow::updateGame()
{
    if (sceneState != GameScene || waitingReset || waitingNextLevel)
    {
        update();
        return;
    }

    int dir = 0;
    if (keyLeft && !keyRight)
    {
        dir = -1;
    }
    else if (keyRight && !keyLeft)
    {
        dir = 1;
    }

    for (int i = 0; i < circles.size(); i++)
    {
        updateRole(circles[i], dir);
    }

    for (int i = 0; i < squares.size(); i++)
    {
        updateRole(squares[i], dir);
    }

    if (!movingSpikes.isEmpty())
    {
        if (!movingSpikesStarted)
        {
            movingSpikeDelayFrames++;

            if (movingSpikeDelayFrames >= 60)
            {
                movingSpikesStarted = true;
            }
        }
        else
        {
            updateMovingSpikes();
        }
    }

    if (jumpBufferFrames > 0)
    {
        bool jumped = false;

        for (int i = 0; i < circles.size(); i++)
        {
            if (circles[i].alive && !circles[i].escaped && circles[i].onGround)
            {
                circles[i].vy = -jumpSpeed;
                circles[i].onGround = false;
                jumped = true;
            }
        }

        for (int i = 0; i < squares.size(); i++)
        {
            if (squares[i].alive && squares[i].onGround)
            {
                squares[i].vy = -jumpSpeed;
                squares[i].onGround = false;
                jumped = true;
            }
        }

        if (jumped)
        {
            jumpBufferFrames = 0;
        }
        else
        {
            jumpBufferFrames--;
        }
    }

    updateHiddenSpikes();

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
                killCircleAndReset(i);
                return;
            }
        }
    }

    for (int i = 0; i < circles.size(); i++)
    {
        if (circles[i].alive && !circles[i].escaped && roleHitSpike(circles[i]))
        {
            killCircleAndReset(i);
            return;
        }
    }

    for (int i = 0; i < squares.size(); i++)
    {
        if (squares[i].alive && roleHitSpike(squares[i]))
        {
            squares[i].alive = false;
        }
    }

    doorOpen = doorOpenAtStart || allSquaresDead();

    if (doorOpen)
    {
        QRect doorRect(doorX, doorY, doorW, doorH);

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

        if (allCirclesEscaped())
        {
            if (currentLevel >= totalLevels)
            {
                showFinishScene();
            }
            else
            {
                scheduleNextLevel();
            }
            return;
        }
    }

    update();
}

#include "player.h"
#include "appwindow.h"

namespace SpiralFun {

Player::Player(AppWindow* appWin) :
    mAppWin(appWin)
{
    QObject::connect(&mTimer, &QTimer::timeout, this, &Player::advance);
}

void Player::play()
{
    mStartTime = QTime::currentTime().msecsSinceStartOfDay();
    mTimer.start(0);
}

void Player::advance()
{
    for (unsigned step = 0.0; step < mStepsPerInterval; ++step)
    {
        mAppWin->advanceCircles(mStepAngle);
        mAngle += mStepAngle;

        if (mAngle >= M_PI * 2)
        {
            mAppWin->removeCirclesFromScene();
            mTimer.disconnect();
            const int stopTime = QTime::currentTime().msecsSinceStartOfDay();
            qDebug() << "Play duration:" << (stopTime - mStartTime) / 1000.0 << "secs";
            break;
        }
    }
}

}

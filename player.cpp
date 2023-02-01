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
    mTimer.start(mStepIntervalMs);
}

void Player::advance()
{
    for (unsigned step = 0.0; step < mStepsPerInterval; ++step)
    {
        mAppWin->advanceCircles(mStepAngle);
        mAngle += mStepAngle;

        if (mAngle >= M_PI * 2)
        {
            mTimer.disconnect();
            break;
        }
    }
}

}

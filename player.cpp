// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "player.h"
#include <chrono>

using namespace std::chrono_literals;

namespace SpiralFun {

Player::Player(const CircleList &circles) :
    mCircles(circles)
{
    QObject::connect(&mPlayTimer, &QTimer::timeout, this, &Player::advance);
    QObject::connect(&mSceneRefreshTimer, &QTimer::timeout, this, [this]{ emit refreshScene(); });
}

void Player::play()
{
    for (auto& circle : mCircles)
        circle->preparePlay();

    mStartTime = QTime::currentTime().msecsSinceStartOfDay();
    mCycles = 0;
    mPlayTimer.start(0);
    mSceneRefreshTimer.start(40ms);
}

void Player::advance()
{
    ++mCycles;
    for (unsigned step = 0.0; step < mStepsPerInterval; ++step)
    {
        advanceCircles(mStepAngle);
        mAngle += mStepAngle;

        if (mAngle >= M_PI * 2)
        {
            mPlayTimer.disconnect();
            mSceneRefreshTimer.disconnect();

            const qreal t = (QTime::currentTime().msecsSinceStartOfDay() - mStartTime) / 1000.0;
            const qreal avgCycleT = t / mCycles;
            qInfo() << "Play duration:" << t << "secs" << "cycles:" << mCycles <<
                        "avg-cycle-time:" << avgCycleT * 1000 << "ms";

            // Draw the last line to close the curve. It may not have been drawn yet
            // due to the minimum draw length.
            forceDraw();
            emit refreshScene();
            emit done();
            break;
        }
    }
}

void Player::advanceCircles(qreal angle)
{
    for(unsigned i = 1; i < mCircles.size(); ++i)
        advanceCircle(i, angle);
}

void Player::advanceCircle(unsigned index, qreal angle)
{
    if (index < 1 || index >= mCircles.size())
        throw std::out_of_range(std::string("index=") + std::to_string(index));

    const QPointF& rotationCenter = mCircles[index - 1]->getCenter();
    const int speed = mCircles[index]->getSpeed();
    const bool clockwise = speed > 0;

    for (unsigned i = index; i < mCircles.size(); ++i)
    {
        auto& circle = *mCircles[i];
        for (int n = 0; n < std::abs(speed); ++n)
            circle.rotate(rotationCenter, angle, clockwise);
    }
}

void Player::forceDraw()
{
    for (auto& circle : mCircles)
    {
        if (circle->getDraw())
            circle->forceDrawToCenter();
    }
}

}

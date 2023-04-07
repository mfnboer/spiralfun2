// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "player.h"
#include <QTime>
#include <chrono>

using namespace std::chrono_literals;

namespace SpiralFun {

Player::Player(const CircleList &circles) :
    mCircles(circles)
{
    mPlayTimer.setInterval(0);
    QObject::connect(&mPlayTimer, &QTimer::timeout, this, &Player::advance);
    mSceneRefreshTimer.setInterval(40ms);
    QObject::connect(&mSceneRefreshTimer, &QTimer::timeout, this, [this]{ emit refreshScene(); });
}

bool Player::play(std::unique_ptr<SceneGrabber> sceneGrabber)
{
    for (auto& circle : mCircles)
        circle->preparePlay();

    mStepsPerInterval = 1;
    mStartTime = QTime::currentTime().msecsSinceStartOfDay();
    mCycles = 0;
    mSceneGrabber = std::move(sceneGrabber);

    if (mSceneGrabber)
        mGifRecorder = std::make_unique<GifRecorder>(mSceneGrabber.get());

    startTimers();

    if (mGifRecorder)
    {
        if (!setupRecording())
            return false;
    }

    return true;
}

void Player::playAll()
{
    for (auto& circle : mCircles)
        circle->preparePlay();

    mStepsPerInterval = 100;
    mStartTime = QTime::currentTime().msecsSinceStartOfDay();
    mCycles = 0;
    mPlayTimer.start();
}

void Player::startTimers()
{
    mPlayTimer.start();
    mSceneRefreshTimer.start();
}

void Player::stopTimers()
{
    mPlayTimer.stop();
    mSceneRefreshTimer.stop();
}

void Player::advance()
{
    ++mCycles;
    for (unsigned step = 0.0; step < mStepsPerInterval; ++step)
    {
        advanceCircles(mStepAngle);
        mAngle += mStepAngle;
        emit angleChanged();

        if (mAngle >= M_PI * 2)
        {
            finishPlaying();
            break;
        }

        if (mRecording)
            record();
    }
}

void Player::finishPlaying()
{
    stopTimers();
    const qreal t = (QTime::currentTime().msecsSinceStartOfDay() - mStartTime) / 1000.0;
    const qreal avgCycleT = t / mCycles;
    qInfo() << "Play duration:" << t << "secs" << "cycles:" << mCycles <<
                "avg-cycle-time:" << avgCycleT * 1000 << "ms";

    Stats stats;
    stats.mCycles = mCycles;
    stats.mPlayTime = std::lround(t * 1000) * 1ms;

    // Draw the last line to close the curve. It may not have been drawn yet
    // due to the minimum draw length.
    forceDraw();
    emit refreshScene();

    if (mRecording)
    {
        // Record last frame
        updateRecordingRect();

        const bool frameAdded = mGifRecorder->addFrame(mRecordingRect, [this, stats]{
                mGifRecorder->stopRecording(true);
                emit done(stats);
            });

        if (!frameAdded)
        {
            qWarning() << "Failed to add last frame";
            mGifRecorder->stopRecording(true);
            emit done(stats);
        }
    }
    else
    {
        emit done(stats);
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

bool Player::setupRecording()
{
    Q_ASSERT(mGifRecorder);
    if (!mGifRecorder->startRecording(GifRecorder::FPS_25))
        return false;

    mFullFrameRect = mGifRecorder->getFullFrameRect();

    // Capture one full frame, next frames will be subframes where changes happened.
    mRecordingRect = mFullFrameRect;
    mRecordAngle = mRecordAngleThreshold;
    mRecording = true;
    record();

    return true;
}

void Player::resetRecordingRect()
{
    mRecordingRect = mSceneGrabber->calcBoundingRectangle(mCircles) & mFullFrameRect;
}

void Player::updateRecordingRect()
{
    mRecordingRect |= mSceneGrabber->calcBoundingRectangle(mCircles) & mFullFrameRect;
}

void Player::record()
{
    updateRecordingRect();
    mRecordAngle += mStepAngle;

    if (mRecordAngle < mRecordAngleThreshold)
        return;

    mRecordAngle = 0.0;
    const bool frameAdded = mGifRecorder->addFrame(mRecordingRect, [this]{ mPlayTimer.start(); });

    if (!frameAdded)
    {
        qDebug() << "Could not add frame";
        return;
    }

    resetRecordingRect();
    mPlayTimer.stop();
}

}

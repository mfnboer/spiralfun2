// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "player.h"
#include "exception.h"
#include "utils.h"
#include <QFile>
#include <QThread>
#include <chrono>

using namespace std::chrono_literals;

namespace SpiralFun {

namespace {
constexpr int FRAME_DURATION = 4; // unit is 10ms
constexpr int GIF_QUALITY = 10;
}

Player::Player(const CircleList &circles) :
    mCircles(circles)
{
    mPlayTimer.setInterval(0);
    QObject::connect(&mPlayTimer, &QTimer::timeout, this, &Player::advance);
    mSceneRefreshTimer.setInterval(40ms);
    QObject::connect(&mSceneRefreshTimer, &QTimer::timeout, this, [this]{ emit refreshScene(); });
}

Player::~Player()
{
    if (mRecordingThread)
    {
        qDebug() << "Wait for recording thread to finish";
        mRecordingThread->wait();
    }

    if (mRecording)
    {
        qDebug() << "Stop recording and remove file:" << mGifFileName;
        stopRecording();
        QFile::remove(mGifFileName);
    }
}

bool Player::play(std::unique_ptr<SceneGrabber> sceneGrabber)
{
    for (auto& circle : mCircles)
        circle->preparePlay();

    mStartTime = QTime::currentTime().msecsSinceStartOfDay();
    mCycles = 0;
    mSceneGrabber = std::move(sceneGrabber);
    startTimers();

    if (mSceneGrabber)
    {
        if (!setupRecording())
            return false;
    }

    return true;
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

    // Draw the last line to close the curve. It may not have been drawn yet
    // due to the minimum draw length.
    forceDraw();
    emit refreshScene();

    if (mRecording)
    {
        // Record last frame
        updateRecordingRect();
        calcFramePosition();

        const bool grabbed = mSceneGrabber->grabScene(mRecordingRect.toRect(),
            [this](QImage&& img){
                qDebug() << "START finalizing video";
                mFrame = std::make_unique<QImage>(std::forward<QImage>(img));
                runRecordFrameThread([this]{
                        stopRecording();
                        qDebug() << "STOP finalizing video";
                        Utils::scanMediaFile(mGifFileName);
                        emit done();
                    });
            });

        if (!grabbed)
        {
            qWarning() << "Failed to grab last frame";
            stopRecording();
            Utils::scanMediaFile(mGifFileName);
            emit done();
        }
    }
    else
    {
        emit done();
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
    Q_ASSERT(mSceneGrabber);
    QString path;
    try {
        path = Utils::getPicturesPath();
    } catch (RuntimeException& e) {
        qWarning() << "Cannot record:" << e.msg();
        return false;
    }

    const QString baseName = Utils::createDateTimeName();
    mGifFileName = path + QString("/VID_%1.gif").arg(baseName);
    mGifEncoder = std::make_unique<GifEncoder>();
    mFullFrameRect = mSceneGrabber->getSpiralCutRect();
    const int width = mFullFrameRect.width();
    const int height = mFullFrameRect.height();

    qDebug() << "Start recording frame:" << mFullFrameRect.size();

    if (!mGifEncoder->open(mGifFileName.toStdString(), width, height, GIF_QUALITY, 0))
    {
        qWarning() << "Cannot open file:" << mGifFileName;
        return false;
    }

    // Capture one full frame, next frames will be subframes where changes happened.
    mRecordingRect = mFullFrameRect;
    mRecordAngle = mRecordAngleThreshold;
    mRecording = true;
    record();

    return true;
}

void Player::stopRecording()
{
    Q_ASSERT(mGifEncoder);
    mGifEncoder->close();
    mRecording = false;
}

void Player::resetRecordingRect()
{
    mRecordingRect = mSceneGrabber->calcBoundingRectangle(mCircles);
}

void Player::updateRecordingRect()
{
    mRecordingRect |= mSceneGrabber->calcBoundingRectangle(mCircles);
}

void Player::calcFramePosition()
{
    mFramePosition = mRecordingRect.translated(-mFullFrameRect.topLeft()).toRect().topLeft();
}

void Player::record()
{
    updateRecordingRect();
    mRecordAngle += mStepAngle;

    if (mRecordAngle < mRecordAngleThreshold)
        return;

    mRecordAngle = 0.0;
    calcFramePosition();

    const bool grabbed = mSceneGrabber->grabScene(mRecordingRect.toRect(),
        [this](QImage&& img){
            mFrame = std::make_unique<QImage>(std::forward<QImage>(img));
            runRecordFrameThread([this]{ mPlayTimer.start(); });
        });

    if (!grabbed)
    {
        qDebug() << "Could not grab frame";
        return;
    }

    resetRecordingRect();
    mPlayTimer.stop();
}

void Player::recordFrame()
{
    Q_ASSERT(mGifEncoder);
    Q_ASSERT(mFrame);
    const uint8_t* frame = mFrame->constBits();
    mGifEncoder->push(frame, mFramePosition.x(), mFramePosition.y(),
                      mFrame->width(), mFrame->height(), FRAME_DURATION);
}

void Player::runRecordFrameThread(const std::function<void()>& whenFinished)
{
    QThread* thread = QThread::create([this]{ recordFrame(); });
    mRecordingThread.reset(thread);
    QObject::connect(thread, &QThread::finished, this, whenFinished, Qt::SingleShotConnection);
    mRecordingThread->start();
}

}

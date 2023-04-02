// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include "gif_recorder.h"
#include <QTimer>

namespace SpiralFun {

class Player : public QObject
{
    Q_OBJECT

public:
    struct Stats
    {
        int mCycles;
        std::chrono::milliseconds mPlayTime;
    };

    explicit Player(const CircleList &circles);

    bool play(std::unique_ptr<SceneGrabber> sceneGrabber = nullptr);
    void playAll();
    qreal getAngle() const { return mAngle; }
    const QString& getGifFileName() const { return mGifRecorder->getFileName(); }

signals:
    void done(Player::Stats stats);
    void refreshScene();
    void angleChanged();

private:
    void startTimers();
    void stopTimers();
    void advance();
    void advanceCircles(qreal angle);
    void advanceCircle(unsigned index, qreal angle);
    void forceDraw();
    void finishPlaying();
    bool setupRecording();
    void resetRecordingRect();
    void updateRecordingRect();
    void record();

    const CircleList& mCircles;
    QTimer mPlayTimer;
    QTimer mSceneRefreshTimer;
    qreal mAngle = 0.0;
    const qreal mStepAngle = qDegreesToRadians(0.05);
    unsigned mStepsPerInterval = 1;
    const qreal mRecordAngleThreshold = qDegreesToRadians(1);
    qreal mRecordAngle = 0.0;
    int mStartTime;
    int mCycles;
    QRect mFullFrameRect;
    QRectF mRecordingRect;
    std::unique_ptr<SceneGrabber> mSceneGrabber;
    std::unique_ptr<GifRecorder> mGifRecorder;
    bool mRecording = false;
};

}

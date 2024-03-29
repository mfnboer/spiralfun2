// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include "recorder.h"
#include <QTimer>

namespace SpiralFun {

using namespace std::chrono_literals;

class Player : public QObject
{
    Q_OBJECT

public:
    struct Stats
    {
        int mCycles = 0;
        std::chrono::milliseconds mPlayTime = 0ms;
        bool mRecordingFailed = false;
    };

    explicit Player(const CircleList &circles);
    ~Player();

    bool play(std::unique_ptr<Recorder> recorder = nullptr);
    void playAll();
    qreal getAngle() const { return mAngle; }
    const QString& getFileName() const { return mRecorder->getFileName(); }

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
    void recordingFailed();
    void finishPlaying();
    bool setupRecording();
    void resetRecordingRect();
    void updateRecordingRect();
    bool record();

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
    std::unique_ptr<Recorder> mRecorder;
    bool mRecording = false;
};

}

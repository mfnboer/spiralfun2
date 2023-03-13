// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include "scene_grabber.h"
#include "egif/GifEncoder.h"
#include <QTime>
#include <QTimer>

namespace SpiralFun {

class Player : public QObject
{
    Q_OBJECT

public:
    explicit Player(const CircleList &circles);
    ~Player();

    bool play(std::unique_ptr<SceneGrabber> sceneGrabber = nullptr);

signals:
    void done();
    void refreshScene();

private:
    void startTimers();
    void stopTimers();
    void advance();
    void advanceCircles(qreal angle);
    void advanceCircle(unsigned index, qreal angle);
    void forceDraw();
    void finishPlaying();
    bool setupRecording();
    void record();
    void recordFrame(const QImage& img);
    void stopRecording();

    const CircleList& mCircles;
    QTimer mPlayTimer;
    QTimer mSceneRefreshTimer;
    qreal mAngle = 0.0;
    const qreal mStepAngle = qDegreesToRadians(0.05);
    const unsigned mStepsPerInterval = 1;
    const qreal mRecordAngleThreshold = qDegreesToRadians(1);
    qreal mRecordAngle = 0.0;
    int mStartTime;
    int mCycles;
    std::unique_ptr<SceneGrabber> mSceneGrabber;
    std::unique_ptr<GifEncoder> mGifEncoder;
    QString mGifFileName;
    bool mRecording = false;
};

}

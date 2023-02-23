// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include <QTime>
#include <QTimer>


namespace SpiralFun {

class Player : public QObject
{
    Q_OBJECT

public:
    Player(const CircleList &circles);

    void play();

signals:
    void done();
    void refreshScene();

private:
    void advance();
    void advanceCircles(qreal angle);
    void advanceCircle(unsigned index, qreal angle);
    void forceDraw();

    const CircleList& mCircles;
    QTimer mPlayTimer;
    QTimer mSceneRefreshTimer;
    qreal mAngle = 0.0;
    const qreal mStepAngle = qDegreesToRadians(0.05);
    const unsigned mStepsPerInterval = 1;
    int mStartTime;
};

}

// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include <QTime>
#include <QTimer>
#include <chrono>

namespace SpiralFun {

using namespace std::chrono_literals;

class AppWindow;

class Player : public QObject
{
    Q_OBJECT

public:
    Player(AppWindow* appWin);

    void play();

private:
    void advance();

    AppWindow* mAppWin;
    QTimer mTimer;
    qreal mAngle = 0.0;
    const qreal mStepAngle = qDegreesToRadians(0.05);
    const unsigned mStepsPerInterval = 32;
    int mStartTime;
};

}

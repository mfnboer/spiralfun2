#pragma once

#include <QTimer>

namespace SpiralFun {

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
    qreal mStepAngle = qDegreesToRadians(0.05);
    unsigned mStepIntervalMs = 1;
    unsigned mStepsPerInterval = 4;
};

}

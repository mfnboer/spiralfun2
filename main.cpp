#include "appwindow.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SpiralFun::AppWindow appWin;
    appWin.addCircle(150);
    appWin.addCircle(80)->setSpeed(1);
    appWin.addCircle(40)->setSpeed(-3);
    appWin.addCircle(20)->setSpeed(9);
    appWin.addCircle(10)->setSpeed(-27);
    appWin.addCircle(5)->setSpeed(81);
    appWin.addCircle(1)->setSpeed(-243)->setDraw(true);
    appWin.showMaximized();
    appWin.show();

    QTimer timer;
    qreal angle = 0.0;
    const qreal stepAngle = qDegreesToRadians(0.05);
    const unsigned stepsPerInterval = 4;
    QObject::connect(&timer, &QTimer::timeout, &appWin, [&appWin,&timer,&angle,stepAngle]{
        for (unsigned step = 0.0; step < stepsPerInterval; ++step)
        {
            appWin.advanceCircles(stepAngle);
            angle += stepAngle;

            if (angle >= M_PI * 2)
            {
                timer.disconnect();
                break;
            }
        }
    });
    timer.start(1);

    return a.exec();
}

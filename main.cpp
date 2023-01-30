#include "mainwindow.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWin;
    mainWin.addCircle(150);
    mainWin.addCircle(80)->setSpeed(1);
    mainWin.addCircle(40)->setSpeed(-3);
    mainWin.addCircle(20)->setSpeed(9);
    mainWin.addCircle(10)->setSpeed(-27);
    mainWin.addCircle(5)->setSpeed(81);
    mainWin.addCircle(1)->setSpeed(-243)->setDraw(true);
    mainWin.show();

    QTimer timer;
    qreal angle = 0.0;
    const qreal stepAngle = qDegreesToRadians(0.5);
    QObject::connect(&timer, &QTimer::timeout, &mainWin, [&mainWin,&timer,&angle,stepAngle]{
        for (double step = 0.0; step < qDegreesToRadians(1); step += stepAngle)
        {
            mainWin.advanceCircles(stepAngle);
            angle += stepAngle;

            if (angle >= M_PI * 2)
            {
                timer.disconnect();
                break;
            }
        }
    });
    timer.start(10);

    return a.exec();
}

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

    return a.exec();
}

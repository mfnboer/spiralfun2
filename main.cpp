#include "appwindow.h"
#include <QApplication>
#include <QTimer>
#include <chrono>

using namespace std::chrono_literals;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qSetMessagePattern("%{time HH:mm:ss.zzz} %{type} %{function}'%{line} %{message}");

    SpiralFun::AppWindow appWin;
    appWin.showMaximized();
    appWin.show();

    // Wait with initialization for window to be shown.
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &timer, [&appWin]{
        appWin.init();
        appWin.setupCircles();
    });
    timer.start(100ms);

    return a.exec();
}

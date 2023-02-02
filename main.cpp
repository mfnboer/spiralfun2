#include "appwindow.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SpiralFun::AppWindow appWin;
    appWin.setupCircles();
    appWin.showMaximized();
    appWin.show();
    qDebug() << "app win:" << appWin.size();

    return a.exec();
}

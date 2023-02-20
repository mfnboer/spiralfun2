// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "appwindow.h"
#include <QApplication>
#include <QQuickStyle>
#include <QQuickView>
#include <QTimer>
#include <chrono>

using namespace std::chrono_literals;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qSetMessagePattern("%{time HH:mm:ss.zzz} %{type} %{function}'%{line} %{message}");

#if 0
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
#endif

    QQuickStyle::setStyle("Material");
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/SpiralFun/main.qml"));
    view.show();
    return app.exec();
}

// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>

using namespace std::chrono_literals;

int main(int argc, char *argv[])
{
    qputenv("QT_SCALE_FACTOR", "0.9");
    QApplication app(argc, argv);
    qSetMessagePattern("%{time HH:mm:ss.zzz} %{type} %{function}'%{line} %{message}");
    QIcon::setThemeName("spiralfun");
    QQmlApplicationEngine engine;
    const QUrl url("qrc:/main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

#if 0
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/SpiralFun/main.qml"));
    view.show();
#endif
    return app.exec();
}

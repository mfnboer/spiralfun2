// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>
#include <QtWebView>

int main(int argc, char *argv[])
{
    // Enable Multisample anti-aliasing (MSAA)
    // More samples is better anti-aliasing. 4 samples seems a reasonable
    // trade-off between quality and performance (on my Galaxy S9).
    auto format = QSurfaceFormat::defaultFormat();
    Q_ASSERT(format.samples() < 4); // Assert to detect if the default ever changes.
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);
    qDebug() << format;

    QtWebView::initialize();
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
    return app.exec();
}

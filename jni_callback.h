// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QObject>

namespace SpiralFun {

class JNICallbackListener : public QObject
{
    Q_OBJECT
public:
    static JNICallbackListener& getInstance();

    void handleViewUriReceived(const QString& uri);
    void handleMediaScannerFinished(const QString contentUri);

signals:
    void viewUriReceived(const QString uri);
    void mediaScannerFinished(const QString contentUri);

private:
    JNICallbackListener();
};

}

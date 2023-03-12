// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QQuickItem>

namespace SpiralFun {

class SceneGrabber : public QObject
{
    Q_OBJECT

public:
    using Callback = std::function<void(const QImage&)>;

    SceneGrabber(QQuickItem* scene, const QRectF& sceneRect);

    bool grabScene(const Callback& callback);

private:
    QQuickItem* mScene;
    const QRectF& mSceneRect;
};

}

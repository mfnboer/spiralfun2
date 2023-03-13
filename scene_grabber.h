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

    void setSceneRect(const QRectF& sceneRect) { mSceneRect = sceneRect; }
    QSize getImageGrabSize() const;
    QSize getSpiralImageSize() const;
    bool grabScene(const Callback& callback);

private:
    QRect getSpiralCutRect() const;
    QImage extractSpiral(const QImage& grabbedImg);

    QQuickItem* mScene;
    QRectF mSceneRect;
};

}

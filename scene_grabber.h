// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include "circle.h"
#include <QQuickItem>

namespace SpiralFun {

class SceneGrabber : public QObject
{
    Q_OBJECT

public:
    using Callback = std::function<void(QImage&&)>;

    SceneGrabber(QQuickItem* scene, const QRectF& sceneRect);

    QRect getSpiralCutRect() const;
    bool grabScene(const Callback& callback);
    bool grabScene(const QRect& cutRect, const Callback& callback);

    // Calculate the bounding rectangle for circles 1..N-1
    // Those are the circles that move.
    // The rectangle position and size are relative to the full scene rect.
    QRectF calcBoundingRectangle(const CircleList& circles) const;

private:
    QSize getImageGrabSize() const;
    QImage extractRect(const QImage& grabbedImg, const QRect& cutRect);

    QQuickItem* mScene;
    QRectF mSceneRect;
    qreal mPixelRatio = 1.0;
};

}

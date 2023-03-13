// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "scene_grabber.h"
#include <QQuickItemGrabResult>
#include <QQuickWindow>

namespace SpiralFun {

namespace {
constexpr int IMG_MARGIN = 20;
}

SceneGrabber::SceneGrabber(QQuickItem* scene, const QRectF& sceneRect) :
    mScene(scene),
    mSceneRect(sceneRect)
{
    Q_ASSERT(scene);
}

QSize SceneGrabber::getImageGrabSize() const
{
    const qreal dpr = mScene->window()->effectiveDevicePixelRatio();
    const QSize imageSize = (mScene->size() * dpr).toSize();
    return imageSize;
}

QSize SceneGrabber::getSpiralImageSize() const
{
    const auto rect = getSpiralCutRect();
    return rect.size();
}

bool SceneGrabber::grabScene(const Callback& callback)
{
    Q_ASSERT(callback);
    auto grabResult = mScene->grabToImage(getImageGrabSize());

    if (!grabResult)
        return false;

    QObject::connect(grabResult.get(), &QQuickItemGrabResult::ready, this,
        [this, grabResult, callback]{
            const QImage img = extractSpiral(grabResult->image());
            callback(img);
        });

    return true;
}

QRect SceneGrabber::getSpiralCutRect() const
{
    const qreal dpr = mScene->window()->effectiveDevicePixelRatio();
    QRectF cutRect = mSceneRect.adjusted(-IMG_MARGIN, -IMG_MARGIN, IMG_MARGIN, IMG_MARGIN);
    cutRect = QRectF(cutRect.topLeft() * dpr, cutRect.size() * dpr);
    return cutRect.toRect();
}

QImage SceneGrabber::extractSpiral(const QImage& grabbedImg)
{
    const QImage spiral = grabbedImg.copy(getSpiralCutRect());
    return spiral;
}

}

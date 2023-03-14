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
    auto* win = scene->window();
    Q_ASSERT(win);
    mPixelRatio = win->effectiveDevicePixelRatio();
}

QSize SceneGrabber::getImageGrabSize() const
{
    const QSize imageSize = (mScene->size() * mPixelRatio).toSize();
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
            QImage img = extractSpiral(grabResult->image());
            callback(std::forward<QImage>(img));
        });

    return true;
}

QRect SceneGrabber::getSpiralCutRect() const
{
    QRectF cutRect = mSceneRect.adjusted(-IMG_MARGIN, -IMG_MARGIN, IMG_MARGIN, IMG_MARGIN);
    cutRect = QRectF(cutRect.topLeft() * mPixelRatio, cutRect.size() * mPixelRatio);
    return cutRect.toRect();
}

QImage SceneGrabber::extractSpiral(const QImage& grabbedImg)
{
    const QImage spiral = grabbedImg.copy(getSpiralCutRect());
    return spiral;
}

}

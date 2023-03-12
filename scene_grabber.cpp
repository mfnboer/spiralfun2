// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "scene_grabber.h"
#include "utils.h"
#include <QQuickItemGrabResult>
#include <QQuickWindow>

namespace SpiralFun {

SceneGrabber::SceneGrabber(QQuickItem* scene, const QRectF& sceneRect) :
    mScene(scene),
    mSceneRect(sceneRect)
{
    Q_ASSERT(scene);
}

bool SceneGrabber::grabScene(const Callback& callback)
{
    Q_ASSERT(callback);
    const qreal dpr = mScene->window()->effectiveDevicePixelRatio();
    const QSize imageSize = (mScene->size() * dpr).toSize();
    auto grabResult = mScene->grabToImage(imageSize);

    if (!grabResult)
        return false;

    QObject::connect(grabResult.get(), &QQuickItemGrabResult::ready, this,
        [this, grabResult, callback, dpr]{
            const QImage img = Utils::extractSpiral(grabResult->image(), mSceneRect, 20, dpr);
            callback(img);
        });

    return true;
}

}

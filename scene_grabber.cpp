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

bool SceneGrabber::grabScene(const Callback& callback)
{
    return grabScene(getSpiralCutRect(), callback);
}

bool SceneGrabber::grabScene(const QRect& cutRect, const Callback& callback)
{
    Q_ASSERT(callback);

    // Make sure all rendering is done before grabbing
    mScene->update();

    auto grabResult = mScene->grabToImage(getImageGrabSize());

    if (!grabResult)
        return false;

    QObject::connect(grabResult.get(), &QQuickItemGrabResult::ready, this,
        [this, grabResult, cutRect, callback]{
            QImage img = extractRect(grabResult->image(), cutRect);
            callback(std::forward<QImage>(img));
        });

    return true;
}

QRectF SceneGrabber::calcBoundingRectangle(const CircleList& circles) const
{
    QRectF rect = circles[1]->boundingRect();
    rect = QRectF(QPointF(0, 0), rect.size() * mPixelRatio);
    rect.translate(circles[1]->x() * mPixelRatio, circles[1]->y() * mPixelRatio);

    for (unsigned i = 2; i < circles.size(); ++i)
    {
        const auto& c = circles[i];
        auto r = c->boundingRect();
        r = QRectF(QPointF(0, 0), r.size() * mPixelRatio);
        r.translate(c->x() * mPixelRatio, c->y() * mPixelRatio);
        rect |= r;
    }

    return rect;
}

QRect SceneGrabber::getSpiralCutRect() const
{
    QRectF cutRect = mSceneRect.adjusted(-IMG_MARGIN, -IMG_MARGIN, IMG_MARGIN, IMG_MARGIN);
    cutRect = QRectF(cutRect.topLeft() * mPixelRatio, cutRect.size() * mPixelRatio);
    return cutRect.toRect();
}

QImage SceneGrabber::extractRect(const QImage& grabbedImg, const QRect& cutRect)
{
    return grabbedImg.copy(cutRect);
}

}

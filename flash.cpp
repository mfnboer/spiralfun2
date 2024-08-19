// Copyright (C) 2024 Michel de Boer
// License: GPLv3
#include "flash.h"
#include "spiral_scene.h"
#include <QPainter>

using namespace std::chrono_literals;

namespace SpiralFun {

Flash::Flash(const QPointF& center, const QColor& color, SpiralScene* parent) :
    QQuickPaintedItem(parent),
    mCenter(center),
    mColor(color)
{
    mTimer.setInterval(50ms);
    connect(&mTimer, &QTimer::timeout, this, [this]{ shrink(); });
    mTimer.start();

    setAntialiasing(true);
    setGeometry();
    setVisible(true);
}

void Flash::setGeometry()
{
    setX(mCenter.x() - mRadius);
    setY(mCenter.y() - mRadius);
    setSize({ 2.0 * mRadius, 2.0 * mRadius });
    update();
}

void Flash::paint(QPainter* painter)
{
    QPen pen(mColor, 1, Qt::SolidLine, Qt::RoundCap);
    painter->setPen(pen);
    painter->setBrush(mColor);
    const QRectF r(0, 0, mRadius * 2.0, mRadius * 2.0);
    painter->drawEllipse(r);
}

void Flash::shrink()
{
    if (--mRadius <= 1)
    {
        mTimer.stop();
        delete this;
        return;
    }

    setGeometry();
}

}

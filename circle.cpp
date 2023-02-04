#include "circle.h"
#include <QtMath>
#include <QScreen>

namespace SpiralFun {

Circle::Circle(QGraphicsView* view, const QPointF& center, qreal radius) :
    mScene(view->scene()),
    mCenter(center),
    mDrawPos(center),
    mRadius(radius)
{
    mEllipse.reset(mScene->addEllipse(getBoundingRect(), QPen(mColor)));
    mEllipse->setPos(mCenter);
}

Circle* Circle::setCenter(const QPointF& center)
{
    moveTo(center);
    mDrawPos = center;
    return this;
}

Circle* Circle::setRadius(qreal radius)
{
    mRadius = radius;
    mEllipse->setRect(getBoundingRect());
    return this;
}

Circle* Circle::setColor(const QColor& color)
{
    mColor = color;
    auto pen = mEllipse->pen();
    pen.setColor(mColor);
    mEllipse->setPen(pen);
    return this;
}

void Circle::setFocus(bool focus)
{
    auto pen = mEllipse->pen();
    pen.setWidth(focus ? 8 : 1);
    mEllipse->setPen(pen);
}

void Circle::rotate(const QPointF& rotationCenter, qreal angle, bool clockwise)
{
    const QLineF line(rotationCenter, mCenter);
    const qreal d = line.length();
    const qreal oldAngle = line.angle();
    const qreal newAngle = qDegreesToRadians(oldAngle) + angle * (clockwise ? -1 : 1);
    const qreal x = qCos(newAngle) * d + rotationCenter.x();
    const qreal y = -qSin(newAngle) * d + rotationCenter.y();

    const QPointF newCenter(x, y);
    moveTo(newCenter);

    if (mDraw)
        drawTo(newCenter);
}

void Circle::moveTo(const QPointF& center)
{
    mCenter = center;
    mEllipse->setPos(mCenter);
}

void Circle::drawTo(const QPointF& center, bool force)
{
    const QLineF line(mDrawPos, center);
    if (line.length() >= mMinDrawLength || force)
    {
        mScene->addLine(line, QPen(mColor, 1, Qt::SolidLine, Qt::RoundCap));;
        mDrawPos = center;
    }
}

void Circle::forceDrawToCenter()
{
    drawTo(mCenter, true);
}

void Circle::removeFromScene()
{
    if (mIsOnScene)
    {
        mScene->removeItem(mEllipse.get());
        mIsOnScene = false;
    }
}

void Circle::addToScene()
{
    if (!mIsOnScene)
    {
        mScene->addItem(mEllipse.get());
        mIsOnScene = true;
    }
}

}

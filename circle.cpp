#include "circle.h"
#include <QtMath>
#include <QGraphicsEllipseItem>
#include <QScreen>

namespace SpiralFun {

Circle::Circle(QGraphicsView* view, const QPointF& center, qreal radius) :
    mScene(view->scene()),
    mCenter(center),
    mDrawPos(center),
    mRadius(radius),
    mMinDrawLength(1.0)
{
    mEllise = mScene->addEllipse(-radius, -radius, radius * 2.0, radius * 2.0);
    mEllise->setPos(mCenter);
}

void Circle::rotate(const QPointF& rotationCenter, qreal angle, bool clockwise)
{
    const QLineF line(rotationCenter, mCenter);
    const qreal d = line.length();
    const qreal oldAngle = line.angle();
    const qreal newAngle = qDegreesToRadians(oldAngle) + angle * (clockwise ? -1 : 1);
    const qreal x = qCos(newAngle) * d + rotationCenter.x();
    const qreal y = -qSin(newAngle) * d + rotationCenter.y();
    moveTo(QPointF(x, y));
}

void Circle::moveTo(const QPointF& center)
{
    if (mDraw)
    {
        const QLineF line(mDrawPos, center);
        if (line.length() >= mMinDrawLength)
        {
            mScene->addLine(line);
            mDrawPos = center;
        }
    }

    mCenter = center;
    mEllise->setPos(mCenter);
}

}

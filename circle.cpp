#include "circle.h"
#include <QGraphicsScene>
#include <QtMath>

namespace SpiralFun {

Circle::Circle(QGraphicsScene* scene, QPointF center, qreal radius) :
    mScene(scene),
    mCenter(center),
    mRadius(radius)
{
    mEllise = scene->addEllipse(-radius, -radius, radius * 2.0, radius * 2.0);
    mEllise->setPos(mCenter);
}

void Circle::rotate(QPointF rotationCenter, qreal angle, bool clockwise)
{
    const QLineF line(rotationCenter, mCenter);
    const qreal d = line.length();
    const qreal oldAngle = line.angle();
    const qreal newAngle = qDegreesToRadians(oldAngle) + angle * (clockwise ? -1 : 1);
    const qreal x = qCos(newAngle) * d + rotationCenter.x();
    const qreal y = -qSin(newAngle) * d + rotationCenter.y();
    moveTo(QPointF(x, y));
}

void Circle::moveTo(QPointF center)
{
    if (mDraw)
        mScene->addLine(QLineF(mCenter, center));

    mCenter = center;
    mEllise->setPos(mCenter);
}

}

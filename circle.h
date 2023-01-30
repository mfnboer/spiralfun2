#pragma once

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

namespace SpiralFun {

class Circle
{
public:
    Circle(QGraphicsScene* scene, QPointF center, qreal radius);

    const QPointF& getCenter() const { return mCenter; }
    qreal getRadius() const { return mRadius; }
    int getSpeed() const { return mSpeed; }
    Circle* setDraw(bool draw) { mDraw = draw; return this; }
    Circle* setSpeed(int speed) { mSpeed = speed; return this; }
    void rotate(QPointF rotationCenter, qreal angle, bool clockwise = true);

private:
    void moveTo(QPointF center);

    QGraphicsScene* mScene;
    QGraphicsEllipseItem* mEllise;
    QPointF mCenter;
    qreal mRadius;
    bool mDraw = false;
    int mSpeed = 0;
};

}

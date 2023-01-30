#pragma once

#include <QGraphicsView>

namespace SpiralFun {

class Circle
{
public:
    Circle(QGraphicsView* view, const QPointF& center, qreal radius);

    const QPointF& getCenter() const { return mCenter; }
    qreal getRadius() const { return mRadius; }
    int getSpeed() const { return mSpeed; }
    Circle* setDraw(bool draw) { mDraw = draw; return this; }
    Circle* setSpeed(int speed) { mSpeed = speed; return this; }
    void rotate(const QPointF& rotationCenter, qreal angle, bool clockwise = true);

private:
    void moveTo(const QPointF& center);
    void drawTo(const QPointF& center);

    QGraphicsScene* mScene;
    QGraphicsEllipseItem* mEllise;
    QPointF mCenter;
    QPointF mDrawPos;
    qreal mRadius;
    bool mDraw = false;
    int mSpeed = 0;
    const qreal mMinDrawLength;
};

}

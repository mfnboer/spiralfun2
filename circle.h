#pragma once

#include <QGraphicsEllipseItem>
#include <QGraphicsView>

namespace SpiralFun {

class Circle
{
public:
    Circle(QGraphicsView* view, const QPointF& center, qreal radius);

    const QPointF& getCenter() const { return mCenter; }
    qreal getRadius() const { return mRadius; }
    int getSpeed() const { return mSpeed; }
    bool getDraw() const { return mDraw; }
    QRectF getBoundingRect() const { return QRectF(-mRadius, -mRadius, mRadius * 2.0, mRadius * 2.0); };
    Circle* setDraw(bool draw) { mDraw = draw; return this; }
    Circle* setSpeed(int speed) { mSpeed = speed; return this; }
    Circle* setCenter(const QPointF& center);
    Circle* setRadius(qreal radius);
    void setFocus(bool focus);
    void rotate(const QPointF& rotationCenter, qreal angle, bool clockwise = true);

private:
    void moveTo(const QPointF& center);
    void drawTo(const QPointF& center);

    QGraphicsScene* mScene;
    std::unique_ptr<QGraphicsEllipseItem> mEllipse;
    QPointF mCenter;
    QPointF mDrawPos;
    qreal mRadius;
    bool mDraw = false;
    int mSpeed = 0;
    const qreal mMinDrawLength;
};

}

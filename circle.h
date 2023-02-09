// Copyright (C) 2023 Michel de Boer
// License: GPLv3
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
    const QColor& getColor() const { return mColor; }
    bool getDraw() const { return mDraw; }
    QGraphicsEllipseItem* GetEllipseItem() const { return mEllipse.get(); }
    QRectF getBoundingRect() const { return QRectF(-mRadius, -mRadius, mRadius * 2.0, mRadius * 2.0); };
    Circle* setColor(const QColor& color);
    Circle* setDraw(bool draw) { mDraw = draw; return this; }
    Circle* setSpeed(int speed) { mSpeed = speed; return this; }
    Circle* setCenter(const QPointF& center);
    Circle* setRadius(qreal radius);
    void setFocus(bool focus);
    void rotate(const QPointF& rotationCenter, qreal angle, bool clockwise = true);
    void removeFromScene();
    void addToScene();
    void forceDrawToCenter();

private:
    void moveTo(const QPointF& center);
    void drawTo(const QPointF& center, bool force = false);

    QGraphicsScene* mScene;
    std::unique_ptr<QGraphicsEllipseItem> mEllipse;
    bool mIsOnScene = true;
    QPointF mCenter;
    QPointF mDrawPos;
    qreal mRadius;
    QColor mColor = Qt::white;
    bool mDraw = false;
    int mSpeed = 0;
    const qreal mMinDrawLength = 2.0;
};

}

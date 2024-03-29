// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include "scoped_line.h"
#include <QQuickPaintedItem>

namespace SpiralFun {

class SpiralScene;

class Circle : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int diameter READ getDiameter WRITE setDiameter NOTIFY diameterChanged)
    Q_PROPERTY(int rotations READ getRotations WRITE setRotations NOTIFY rotationsChanged)
    Q_PROPERTY(Direction direction READ getDirection WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(int draw READ getDraw WRITE setDraw NOTIFY drawChanged)
    Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
    QML_ELEMENT

public:
    static constexpr int MAX_DRAW = 7;

    explicit Circle(SpiralScene* parent = nullptr);

    enum Direction { CLOCKWISE = 0, COUNTER_CLOCKWISE = 1 };
    Q_ENUM(Direction)

    const QPointF& getCenter() const { return mCenter; }
    qreal getRadius() const { return mDiameter / 2.0; }
    int getDiameter() const { return mDiameter; }
    int getSpeed() const { return mSpeed; }
    int getRotations() const { return std::abs(getSpeed()); }
    Direction getDirection() const;
    const QColor& getColor() const { return mColor; }
    int getDraw() const { return mDraw; }
    QRectF getBoundingRect() const { return QRectF(-getRadius(), -getRadius(), mDiameter, mDiameter); };
    Circle* setColor(const QColor& color);
    Circle* setDraw(int draw);
    Circle* setSpeed(int speed);
    Circle* setRotations(int rotations);
    Circle* setDirection(Direction direction);
    Circle* setCenter(const QPointF& center);
    Circle* setRadius(qreal radius);
    Circle* setDiameter(int diameter);
    void setEnabled(bool enabled);
    void setFocus(bool focus);
    void rotate(const QPointF& rotationCenter, qreal angle, bool clockwise = true);
    void removeFromScene();
    void addToScene();
    void forceDrawToCenter();
    void preparePlay();

    void paint(QPainter* painter) override;

signals:
    void diameterChanged(int oldDiameter);
    void rotationsChanged();
    void drawChanged();
    void directionChanged();
    void colorChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void moveTo(const QPointF& center);
    void drawTo(const QPointF& center, bool force = false);

    SpiralScene* mScene;
    QPointF mCenter;
    QPointF mDrawPos;
    int mDiameter = 1;
    QColor mColor = Qt::white;
    int mDraw = 0;
    int mSpeed = 0;
    const qreal mMinDrawLength = 2.0;
    int mPenWidth;
    ScopedLine mSceneLine;
};

using CircleList = std::vector<std::unique_ptr<SpiralFun::Circle>>;

}

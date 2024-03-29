// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "circle.h"
#include "spiral_scene.h"
#include <QPainter>
#include <QtMath>

namespace SpiralFun {

namespace {
constexpr int CIRCLE_PEN_WIDTH = 1;
constexpr int SELECT_PEN_WIDTH = 8;
}

Circle::Circle(SpiralScene* parent) :
    QQuickPaintedItem(parent),
    mScene(parent),
    mPenWidth(CIRCLE_PEN_WIDTH)
{
    setEnabled(true);
    setAntialiasing(true);
}

Circle::Direction Circle::getDirection() const
{
    return mSpeed < 0 ? COUNTER_CLOCKWISE : CLOCKWISE;
}

Circle* Circle::setCenter(const QPointF& center)
{
    moveTo(center);
    mDrawPos = center;
    return this;
}

Circle* Circle::setRadius(qreal radius)
{
    Q_ASSERT(radius > 0.0);
    return setDiameter(std::round(radius * 2));
}

Circle* Circle::setDiameter(int diameter)
{
    if (mDiameter != diameter)
    {
        const int oldDiameter = mDiameter;
        mDiameter = diameter;

        // Make the bounding box large too show a selected circle.
        const qreal d = diameter + SELECT_PEN_WIDTH;
        setSize({d, d});
        moveTo(mCenter);
        emit diameterChanged(oldDiameter);
    }

    return this;
}

Circle* Circle::setColor(const QColor& color)
{
    if (mColor != color)
    {
        mColor = color;
        emit colorChanged();
        update();
    }

    return this;
}

Circle* Circle::setDraw(int draw)
{
    Q_ASSERT(draw >= 0);
    Q_ASSERT(draw <= MAX_DRAW);

    if (draw != mDraw)
    {
        mDraw = draw;
        emit drawChanged();
    }

    return this;
}

Circle* Circle::setSpeed(int speed)
{
    const bool rotChanged = (std::abs(speed) != std::abs(mSpeed));
    const bool dirChanged = (speed < 0 && mSpeed >= 0) || (speed >=0 && mSpeed < 0);
    mSpeed = speed;

    if (rotChanged)
        emit rotationsChanged();
    if (dirChanged)
        emit directionChanged();

    return this;
}

Circle* Circle::setRotations(int rotations)
{
    return setSpeed(rotations * (mSpeed < 0 ? -1 : 1));
}

Circle* Circle::setDirection(Direction direction)
{
    const int rotations = std::abs(mSpeed);
    switch (direction)
    {
    case CLOCKWISE:
        return setSpeed(rotations);
    case COUNTER_CLOCKWISE:
        return setSpeed(-rotations);
    }

    assert(false);
    return this;
}

void Circle::setEnabled(bool enabled)
{
    setAcceptedMouseButtons(enabled ? Qt::LeftButton : Qt::NoButton);
}

void Circle::setFocus(bool focus)
{
    mPenWidth = focus ? SELECT_PEN_WIDTH : CIRCLE_PEN_WIDTH;
    update();
}

void Circle::rotate(const QPointF& rotationCenter, qreal angle, bool clockwise)
{
    const QLineF line(rotationCenter, mCenter);
    const qreal d = line.length();
    const qreal oldAngle = qDegreesToRadians(line.angle());
    const qreal newAngle = oldAngle + angle * (clockwise ? -1 : 1);
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

    if (isVisible())
    {
        setX(mCenter.x() - getRadius() - SELECT_PEN_WIDTH / 2.0);
        setY(mCenter.y() - getRadius() - SELECT_PEN_WIDTH / 2.0);
    }
}

void Circle::drawTo(const QPointF& center, bool force)
{
    const QLineF line(mDrawPos, center);
    if (line.length() >= mMinDrawLength || force)
    {
        Q_ASSERT(*mSceneLine);
        mSceneLine->addPoint(center);
        mDrawPos = center;
    }
}

void Circle::forceDrawToCenter()
{
    drawTo(mCenter, true);
}

void Circle::removeFromScene()
{
    setVisible(false);
}

void Circle::addToScene()
{
    setVisible(true);
    moveTo(mCenter);
}

void Circle::paint(QPainter* painter)
{
    QPen pen(mColor, mPenWidth, Qt::SolidLine, Qt::RoundCap);
    painter->setPen(pen);
    const qreal coord = SELECT_PEN_WIDTH / 2.0;
    const QRectF r(coord, coord, qreal(mDiameter), qreal(mDiameter));
    painter->drawEllipse(r);
}

void Circle::preparePlay()
{
    mSceneLine = {};
    if (mDraw)
        mSceneLine = mScene->addLine(this, mColor, mDraw, mCenter);
}

void Circle::mousePressEvent(QMouseEvent *event)
{
    mScene->selectCircle(this);
    event->accept();
}

}

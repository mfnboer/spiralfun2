// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include <QGraphicsView>
#include <QGestureEvent>

namespace SpiralFun {

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(QGraphicsScene* scene = nullptr, QWidget* parent = nullptr);

protected:
    bool event(QEvent* event) override;

private:
    void setTransformation();
    bool gestureEvent(QGestureEvent* event);
    void pinchGesture(QPinchGesture* gesture);

    qreal mTotalScaleFactor = 1.0;
    qreal mCurrentScaleFactor = 1.0;
};

}

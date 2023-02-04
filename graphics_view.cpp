#include "graphics_view.h"
#include <QTouchEvent>

namespace SpiralFun {

GraphicsView::GraphicsView(QGraphicsScene* scene, QWidget* parent) :
    QGraphicsView(scene, parent)
{
    setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(ScrollHandDrag);
    grabGesture(Qt::PinchGesture);
}

void GraphicsView::setTransformation()
{
    setTransform(QTransform::fromScale(mTotalScaleFactor * mCurrentScaleFactor,
                                       mTotalScaleFactor * mCurrentScaleFactor));
}

bool GraphicsView::event(QEvent* event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));

    return QGraphicsView::event(event);
}

bool GraphicsView::gestureEvent(QGestureEvent* event)
{
    qDebug() << "gesture event:" << event;
    if (QGesture* pinch = event->gesture(Qt::PinchGesture))
        pinchGesture(static_cast<QPinchGesture*>(pinch));

    return true;
}

void GraphicsView::pinchGesture(QPinchGesture* gesture)
{
    const auto changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::ScaleFactorChanged)
    {
        mCurrentScaleFactor = gesture->totalScaleFactor();
        setTransformation();
    }
    if (gesture->state() == Qt::GestureFinished)
    {
        mTotalScaleFactor *= mCurrentScaleFactor;
        mCurrentScaleFactor = 1.0;
    }
}

}

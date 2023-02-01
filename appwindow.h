#pragma once

#include "circle.h"
#include <QGraphicsScene>
#include <memory>
#include <vector>

namespace SpiralFun {

class AppWindow : public QWidget
{
    Q_OBJECT

public:
    AppWindow();

    SpiralFun::Circle* addCircle(qreal radius);
    void advanceCircles(qreal angle);

private:
    void advanceCircle(unsigned index, qreal angle);

    QGraphicsView* mView;
    QGraphicsScene* mScene;
    std::vector<std::unique_ptr<SpiralFun::Circle>> mCircles;
};

}

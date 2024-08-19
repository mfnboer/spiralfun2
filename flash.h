// Copyright (C) 2024 Michel de Boer
// License: GPLv3
#pragma once

#include <QTimer>
#include <QQuickPaintedItem>

namespace SpiralFun {

class SpiralScene;

class Flash : public QQuickPaintedItem
{
    Q_OBJECT

public:
    Flash(const QPointF& center, const QColor& color, SpiralScene* parent = nullptr);

    void paint(QPainter* painter) override;

private:
    void setGeometry();
    void shrink();

    QPointF mCenter;
    int mRadius = 8;
    QColor mColor;
    QTimer mTimer;
};

}

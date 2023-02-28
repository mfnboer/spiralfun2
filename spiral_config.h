// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include "circle.h"
#include <QJsonDocument>

namespace SpiralFun {

class SpiralConfig
{
public:
    SpiralConfig(const CircleList& circles, qreal defaultRadius);

    QJsonDocument createJsonDoc() const;
    bool save(const QImage& img) const;

private:
    const CircleList& mCircles;
    const qreal mDefaultRadius;
};

}

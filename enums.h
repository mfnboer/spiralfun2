// Copyright (C) 2025 Michel de Boer
// License: GPLv3
#pragma once
#include <QObject>
#include <QtQmlIntegration>

namespace SpiralFun {

class QEnums : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum InsetsSide
    {
        INSETS_SIDE_TOP,
        INSETS_SIDE_BOTTOM,
        INSETS_SIDE_LEFT,
        INSETS_SIDE_RIGHT
    };
    Q_ENUM(InsetsSide)
};

}

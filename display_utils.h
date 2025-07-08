// Copyright (C) 2025 Michel de Boer
// License: GPLv3
#pragma once
#include "enums.h"

namespace SpiralFun {

class DisplayUtils : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit DisplayUtils(QObject* parent = nullptr);

    Q_INVOKABLE static int getNavigationBarSize(QEnums::InsetsSide side);
    Q_INVOKABLE static int getStatusBarSize(QEnums::InsetsSide side);
    Q_INVOKABLE static int getDisplayCutoutSize(QEnums::InsetsSide side);
};

}

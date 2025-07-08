// Copyright (C) 2025 Michel de Boer
// License: GPLv3
#include "display_utils.h"
#include "utils.h"

namespace SpiralFun {

DisplayUtils::DisplayUtils(QObject* parent) : QObject(parent)
{
}

int DisplayUtils::getNavigationBarSize(QEnums::InsetsSide side)
{
    return Utils::getNavigationBarSize(side);
}

int DisplayUtils::getStatusBarSize(QEnums::InsetsSide side)
{
    return Utils::getStatusBarSize(side);
}

int DisplayUtils::getDisplayCutoutSize(QEnums::InsetsSide side)
{
    return Utils::getDisplayCutoutSize(side);
}

}

// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "scoped_line.h"

namespace SpiralFun {

ScopedLine::ScopedLine(Line* line, const std::function<void()>& cleanup) :
    mLine(line),
    mCleanup(cleanup)
{
}

ScopedLine::~ScopedLine()
{
    if (mCleanup)
        mCleanup();
}

ScopedLine::ScopedLine(ScopedLine&& other)
{
    mLine = other.mLine;
    mCleanup = std::move(other.mCleanup);
    other.mCleanup = nullptr;
}

ScopedLine& ScopedLine::operator=(ScopedLine&& rhs)
{
    if (mCleanup)
        mCleanup();

    mLine = rhs.mLine;
    mCleanup = std::move(rhs.mCleanup);
    rhs.mCleanup = nullptr;
    return *this;
}

}

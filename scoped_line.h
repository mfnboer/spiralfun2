// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QColor>
#include <QPointF>
#include <QSGNode>
#include <functional>
#include <vector>

namespace SpiralFun {

struct Line
{
      std::vector<QPointF> mLinePoints;
      QColor mColor;
      int mLineWidth = 1;
      QSGNode* mRoot = nullptr;
      void addPoint(const QPointF& p) { mLinePoints.push_back(p); }
};

class ScopedLine
{
public:
    ScopedLine() = default;
    ScopedLine(Line* line, const std::function<void()>& cleanup);
    ~ScopedLine();

    ScopedLine(const ScopedLine&) = delete;
    ScopedLine(ScopedLine&&);
    ScopedLine& operator=(const ScopedLine&) = delete;
    ScopedLine& operator=(ScopedLine&&);

    Line* operator*() { return mLine; }
    Line* operator->() { return mLine; }

private:
    Line* mLine = nullptr;
    std::function<void()> mCleanup;
};

}

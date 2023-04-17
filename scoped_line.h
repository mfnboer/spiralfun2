// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QColor>
#include <QPointF>
#include <QSGFlatColorMaterial>
#include <QSGNode>
#include <functional>
#include <vector>

namespace SpiralFun {

class LineMaterial : public QSGFlatColorMaterial
{
public:
    QSGMaterialType* type() const override { return &const_cast<QSGMaterialType&>(mType); }

private:
    QSGMaterialType mType;
};

struct Line
{
      std::vector<QPointF> mLinePoints;
      QColor mColor;
      int mLineWidth = 1;
      LineMaterial* mMaterial = nullptr;
      QSGNode* mRoot = nullptr;

      void addPoint(const QPointF& p) { mLinePoints.push_back(p); }
      void setNodeMaterial(QSGGeometryNode* node);
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

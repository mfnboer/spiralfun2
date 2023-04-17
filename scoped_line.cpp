// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "scoped_line.h"

namespace SpiralFun {

void Line::setNodeMaterial(QSGGeometryNode* node)
{
    // There seems to be an GL bug that causes the line width from one line to
    // be used for drawing another line if they had the same color.
    // By creating a unique material type for each line this bug is avoided as
    // for each unique material a shader will be created.

    if (mMaterial)
    {
        // Subsequent nodes will point to the material owned by the first node.
        node->setMaterial(mMaterial);
        node->setFlag(QSGNode::OwnsMaterial, false);
    }
    else
    {
        // The first node will own the material.
        mMaterial = new LineMaterial;
        mMaterial->setColor(mColor);
        mMaterial->setFlag(QSGMaterial::Blending);
        node->setMaterial(mMaterial);
        node->setFlag(QSGNode::OwnsMaterial, true);
    }
}

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

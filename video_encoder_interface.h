// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QImage>
#include <QString>

namespace SpiralFun {

class IVideoEncoder
{
public:
    virtual ~IVideoEncoder() = default;

    virtual bool open(const QString& fileName, int width, int height, int fps) = 0;
    virtual bool close() = 0;
    virtual bool push(const QImage& frame, int x = 0, int y = 0) = 0;
    virtual QString getFileExtension() const = 0;
};

}

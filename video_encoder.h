// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include "video_encoder_interface.h"
#include <QtGlobal>

#if defined(Q_OS_ANDROID)
#include <QJniObject>
#endif

namespace SpiralFun {

class VideoEncoder : public IVideoEncoder
{
public:
    bool open(const QString &fileName, int width, int height, int fps, int bitsPerFrame) override;
    bool close() override;
    bool push(const QImage& frame, int x = 0, int y = 0) override;
    QString getFileExtension() const override { return "mp4"; }
    bool canEncodePartialFrame() const override { return false; }

private:
#if defined(Q_OS_ANDROID)
    std::unique_ptr<QJniObject> mEncoder;
#endif
    int mWidth = 0;
    int mHeight = 0;
};

}

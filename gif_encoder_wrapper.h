// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include "video_encoder_interface.h"
#include "egif/GifEncoder.h"

namespace SpiralFun {

class GifEncoderWrapper : public IVideoEncoder
{
public:
    bool open(const QString& fileName, int width, int height, int fps) override;
    bool close() override;
    bool push(const QImage& frame, int x, int y) override;

private:
    std::unique_ptr<GifEncoder> mGifEncoder;
    int mFrameDuration = 4;
};

}

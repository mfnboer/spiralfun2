// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "gif_encoder_wrapper.h"

namespace SpiralFun {

namespace {
constexpr int GIF_QUALITY = 10;
constexpr int GIF_LOOP = 0;
}

bool GifEncoderWrapper::open(const QString& fileName, int width, int height, int fps, int)
{
    Q_ASSERT(fps > 0);
    mFrameDuration = 100 / fps;
    mGifEncoder = std::make_unique<GifEncoder>();
    return mGifEncoder->open(fileName.toStdString(), width, height, GIF_QUALITY, GIF_LOOP);
}

bool GifEncoderWrapper::close()
{
    bool result = true;

    if (mGifEncoder)
    {
        result = mGifEncoder->close();
        mGifEncoder = nullptr;
    }

    return result;
}

bool GifEncoderWrapper::push(const QImage& frame, int x, int y)
{
    Q_ASSERT(mGifEncoder);
    const uint8_t* frameBits = frame.constBits();
    return mGifEncoder->push(frameBits, x, y, frame.width(), frame.height(), mFrameDuration);
}

}

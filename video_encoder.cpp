// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "video_encoder.h"

#if not defined(Q_OS_ANDROID)
#include "exception.h"
#endif

namespace SpiralFun {

bool VideoEncoder::open(const QString &fileName, int width, int height, int fps)
{
#if defined(Q_OS_ANDROID)
    Q_ASSERT(!mEncoder);
    mWidth = width;
    mHeight = height;
    auto jsFile = QJniObject::fromString(fileName);
    mEncoder = std::make_unique<QJniObject>("com/gmail/mfnboer/QVideoEncoder");
    const jboolean result = mEncoder->callMethod<jboolean>("init", "(Ljava/lang/String;III)Z",
                                                           jsFile.object<jstring>(),
                                                           (jint)width, (jint)height, (jint)fps);
    return (bool)result;
#else
    Q_UNUSED(fileName);
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(fps);
    throw RuntimeException("Video encoding not supported!");
#endif
}

bool VideoEncoder::close()
{
#if defined(Q_OS_ANDROID)
    if (mEncoder)
    {
        mEncoder->callMethod<void>("release", "()V");
        mEncoder = nullptr;
    }

    return true;
#else
    throw RuntimeException("Video encoding not supported!");
#endif
}

bool VideoEncoder::push(const QImage& frame, int, int)
{
#if defined(Q_OS_ANDROID)
    Q_ASSERT(mWidth == frame.width());
    Q_ASSERT(mHeight == frame.height());
    int size = frame.width() * frame.height() * 4;
    auto jsFrame = mEnv->NewByteArray(size);
    const uint8_t* frameBits = frame.constBits();
    mEnv->SetByteArrayRegion(jsFrame, 0, size, (jbyte*)frameBits);
    mEncoder->callMethod<void>("addFrame", "([B)V", jsFrame);
    return true;
#else
    Q_UNUSED(frame);
    throw RuntimeException("Video encoding not supported!");
#endif
}

}

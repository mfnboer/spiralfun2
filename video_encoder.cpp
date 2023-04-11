// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "video_encoder.h"

#if not defined(Q_OS_ANDROID)
#include "exception.h"
#endif

namespace SpiralFun {

bool VideoEncoder::open(const QString &fileName, int width, int height)
{
#if defined(Q_OS_ANDROID)
    Q_ASSERT(!mEncoder);
    auto jsFile = QJniObject::fromString(fileName);
    mEncoder = std::make_unique<QJniObject>("com/gmail/mfnboer/QVideoEncoder");
    const jboolean result = mEncoder->callMethod<jboolean>("init", "(IILjava/lang/String;)Z",
                                   (jint)width, (jint)height, jsFile.object<jstring>());
    return (bool)result;
#else
    Q_UNUSED(fileName);
    Q_UNUSED(width);
    Q_UNUSED(height);
    throw RuntimeException("Video encoding not supported!");
#endif
}

bool VideoEncoder::close()
{
#if defined(Q_OS_ANDROID)
    if (mEncoder)
        mEncoder->callMethod<void>("release", "()V");

    return true;
#else
    throw RuntimeException("Video encoding not supported!");
#endif
}

bool VideoEncoder::push(const uint8_t* frame)
{
#if defined(Q_OS_ANDROID)
    mEncoder->callMethod<void>("addFrame", "([B)V", (jbyteArray)frame);
    return true;
#else
    Q_UNUSED(frame);
    Q_UNUSED(last);
    throw RuntimeException("Video encoding not supported!");
#endif
}

}

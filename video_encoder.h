// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QtGlobal>

#if defined(Q_OS_ANDROID)
#include <QJniObject>
#endif

namespace SpiralFun {

class VideoEncoder
{
public:
    bool open(const QString &fileName, int width, int height);
    bool close();
    bool push(const uint8_t* frame);

private:
#if defined(Q_OS_ANDROID)
    std::unique_ptr<QJniObject> mEncoder;
#endif
};

}

// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "scene_grabber.h"
#include "egif/GifEncoder.h"
#include <QString>
#include <memory>

namespace SpiralFun {

class GifRecorder : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum FrameRate { FPS_25, FPS_10, FPS_4, FPS_2, FPS_1 };
    Q_ENUM(FrameRate);

    explicit GifRecorder(SceneGrabber& sceneGrabber);
    ~GifRecorder();

    const QRect& getFullFrameRect() const { return mFullFrameRect; }
    const QString& getFileName() const { return mGifFileName; }

    bool startRecording(FrameRate frameRate, const QString& baseNameSuffix = "");
    void stopRecording(bool scanMediaFile);

    using FrameAddedCallback = std::function<void()>;
    bool addFrame(const FrameAddedCallback& frameAddedCallback);
    bool addFrame(const QRectF& rect, const FrameAddedCallback& frameAddedCallback);

private:
    void setFrameDuration(FrameRate frameRate);
    void calcFramePosition(const QRectF& frameRect);
    void recordFrame();
    void runRecordFrameThread(const FrameAddedCallback& whenFinished);

    SceneGrabber& mSceneGrabber;
    std::unique_ptr<GifEncoder> mGifEncoder;
    QString mGifFileName;
    bool mRecording = false;
    std::unique_ptr<QImage> mFrame;
    int mFrameDuration = 4; // unit is 10ms
    int mFrameNumber = 0;
    QRect mFullFrameRect;
    QPoint mFramePosition;
    std::unique_ptr<QThread> mRecordingThread;
};

}

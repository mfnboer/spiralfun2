// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "scene_grabber.h"
#include "video_encoder_interface.h"
#include <QString>
#include <memory>

namespace SpiralFun {

class Recorder : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum FrameRate { FPS_25, FPS_10, FPS_4, FPS_2, FPS_1 };
    Q_ENUM(FrameRate);

    enum Format { FMT_GIF, FMT_VIDEO };
    Q_ENUM(Format);

    static std::unique_ptr<Recorder> createRecorder(Format format, std::unique_ptr<SceneGrabber> sceneGrabber);

    explicit Recorder(std::unique_ptr<SceneGrabber> sceneGrabber = nullptr);
    ~Recorder();

    void setEncoder(std::unique_ptr<IVideoEncoder> encoder) { mEncoder = std::move(encoder); }
    void setBitsPerFrame(int bitsPerFrame) { mBitsPerFrame = bitsPerFrame; }
    const QRect& getFullFrameRect() const { return mFullFrameRect; }
    const QString& getFileName() const { return mFileName; }

    bool startRecording(FrameRate frameRate, const QString& baseNameSuffix = "");
    void stopRecording(bool scanMediaFile);

    using FrameAddedCallback = std::function<void(bool frameAdded)>;
    bool addFrame(const FrameAddedCallback& frameAddedCallback);
    bool addFrame(const QRectF& recordingRect, const FrameAddedCallback& frameAddedCallback);
    static int frameRateToFps(FrameRate frameRate);

    QRectF sceneRectToRecordingRect(const QRectF& sceneRect) const { return mSceneGrabber->getGrabRect(sceneRect); }
    QRectF calcBoundingRectangle(const CircleList& circles) const { return mSceneGrabber->calcBoundingRectangle(circles); }

private:
    void calcFramePosition(const QRectF& frameRect);
    void recordFrame();
    void runRecordFrameThread(const FrameAddedCallback& whenFinished);

    std::unique_ptr<SceneGrabber> mSceneGrabber;
    std::unique_ptr<IVideoEncoder> mEncoder;
    QString mFileName;
    int mBitsPerFrame = 80000;
    bool mRecording = false;
    std::unique_ptr<QImage> mFrame;
    bool mLastFrameAdded = false;
    int mFrameNumber = 0;
    QRect mFullFrameRect;
    QPoint mFramePosition;
    std::unique_ptr<QThread> mRecordingThread;
};

}

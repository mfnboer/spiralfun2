// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "recorder.h"
#include "exception.h"
#include "gif_encoder_wrapper.h"
#include "utils.h"
#include "video_encoder.h"
#include <QFile>
#include <QThread>

namespace SpiralFun {

std::unique_ptr<Recorder> Recorder::createRecorder(Recorder::Format format, std::unique_ptr<SceneGrabber> sceneGrabber)
{
    auto recorder = std::make_unique<Recorder>(std::move(sceneGrabber));
    std::unique_ptr<IVideoEncoder> encoder;

    switch (format)
    {
    case FMT_GIF:
        encoder = std::make_unique<GifEncoderWrapper>();
        break;
    case FMT_VIDEO:
        encoder = std::make_unique<VideoEncoder>();
        break;
    }

    recorder->setEncoder(std::move(encoder));
    return recorder;
}

Recorder::Recorder(std::unique_ptr<SceneGrabber> sceneGrabber) :
    QObject(),
    mSceneGrabber(std::move(sceneGrabber))
{
    if (mSceneGrabber)
        mFullFrameRect = mSceneGrabber->getSpiralCutRect();
}

Recorder::~Recorder()
{
    if (mRecordingThread)
    {
        qDebug() << "Wait for recording thread to finish";
        mRecordingThread->wait();
    }

    if (mRecording)
    {
        qDebug() << "Stop recording and remove file:" << mFileName;
        stopRecording(false);
        QFile::remove(mFileName);
    }
}

bool Recorder::startRecording(FrameRate frameRate, const QString& baseNameSuffix)
{
    Q_ASSERT(mEncoder);

    if (mRecording)
    {
        qWarning() << "Recoring already started!";
        return true;
    }

    QString path;
    try {
        path = Utils::getPicturesPath();
    } catch (RuntimeException& e) {
        qWarning() << "Cannot record:" << e.msg();
        return false;
    }

    const QString baseName = Utils::createDateTimeName();
    const QString ext = mEncoder->getFileExtension();
    mFileName = path + QString("/VID_%1%2.%3").arg(baseName, baseNameSuffix, ext);
    const int width = mFullFrameRect.width();
    const int height = mFullFrameRect.height();

    qDebug() << "Start recording frame:" << mFullFrameRect.size();

    if (!mEncoder->open(mFileName, width, height, frameRateToFps(frameRate), mBitsPerFrame))
    {
        qWarning() << "Cannot open file:" << mFileName;
        return false;
    }

    mRecording = true;
    mFrameNumber = 0;
    return true;
}

void Recorder::stopRecording(bool scanMediaFile)
{
    if (!mRecording)
        return;

    mEncoder->close();
    mRecording = false;

    if (scanMediaFile)
        Utils::scanMediaFile(mFileName);
}

bool Recorder::addFrame(const FrameAddedCallback& frameAddedCallback)
{
    return addFrame(mFullFrameRect.toRectF(), frameAddedCallback);
}

bool Recorder::addFrame(const QRectF& recordingRect, const FrameAddedCallback& frameAddedCallback)
{
    Q_ASSERT(mSceneGrabber);
    const QRect frameRect = (mFrameNumber == 0 || !mEncoder->canEncodePartialFrame()) ?
            mFullFrameRect : recordingRect.toRect();
    calcFramePosition(frameRect);

    const bool grabbed = mSceneGrabber->grabScene(frameRect,
        [this, frameAddedCallback](QImage&& img){
            mFrame = std::make_unique<QImage>(std::forward<QImage>(img));
            runRecordFrameThread([frameAddedCallback](bool added){
                if (frameAddedCallback) frameAddedCallback(added);
            });
        });

    if (!grabbed)
    {
        qDebug() << "Could not grab frame";
        return false;
    }

    ++mFrameNumber;
    return true;
}

int Recorder::frameRateToFps(FrameRate frameRate)
{
    switch (frameRate)
    {
    case FPS_25:
        return 25;
    case FPS_10:
        return 10;
    case FPS_4:
        return 4;
    case FPS_2:
        return 2;
    case FPS_1:
        return 1;
    }

    Q_ASSERT(false);
    return 25;
}

void Recorder::calcFramePosition(const QRectF& frameRect)
{
    mFramePosition = frameRect.translated(-mFullFrameRect.topLeft()).toRect().topLeft();
}

void Recorder::recordFrame()
{
    Q_ASSERT(mEncoder);
    Q_ASSERT(mFrame);
    mLastFrameAdded = mEncoder->push(*mFrame, mFramePosition.x(), mFramePosition.y());
}

void Recorder::runRecordFrameThread(const FrameAddedCallback& whenFinished)
{
    mLastFrameAdded = false;
    QThread* thread = QThread::create([this]{ recordFrame(); });
    mRecordingThread.reset(thread);
    QObject::connect(thread, &QThread::finished, this,
        [this, whenFinished]{ whenFinished(mLastFrameAdded); },
        Qt::SingleShotConnection);
    mRecordingThread->start();
}

}

// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "gif_recorder.h"
#include "exception.h"
#include "utils.h"
#include <QFile>
#include <QThread>

namespace SpiralFun {

namespace {
constexpr int GIF_QUALITY = 10;
constexpr int GIF_LOOP = 0;
}

GifRecorder::GifRecorder(SceneGrabber& sceneGrabber) :
    mSceneGrabber(sceneGrabber),
    mFullFrameRect(mSceneGrabber.getSpiralCutRect())
{
}

GifRecorder::~GifRecorder()
{
    if (mRecordingThread)
    {
        qDebug() << "Wait for recording thread to finish";
        mRecordingThread->wait();
    }

    if (mRecording)
    {
        qDebug() << "Stop recording and remove file:" << mGifFileName;
        stopRecording(false);
        QFile::remove(mGifFileName);
    }
}

bool GifRecorder::startRecording(FrameRate frameRate, const QString& baseNameSuffix)
{
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
    mGifFileName = path + QString("/VID_%1%2.gif").arg(baseName, baseNameSuffix);
    mGifEncoder = std::make_unique<GifEncoder>();
    const int width = mFullFrameRect.width();
    const int height = mFullFrameRect.height();
    setFrameDuration(frameRate);

    qDebug() << "Start recording frame:" << mFullFrameRect.size();

    if (!mGifEncoder->open(mGifFileName.toStdString(), width, height, GIF_QUALITY, GIF_LOOP))
    {
        qWarning() << "Cannot open file:" << mGifFileName;
        return false;
    }

    mRecording = true;
    mFrameNumber = 0;
    return true;
}

void GifRecorder::stopRecording(bool scanMediaFile)
{
    if (!mRecording)
        return;

    mGifEncoder->close();
    mRecording = false;

    if (scanMediaFile)
        Utils::scanMediaFile(mGifFileName);
}

bool GifRecorder::addFrame(const FrameAddedCallback& frameAddedCallback)
{
    return addFrame(mFullFrameRect.toRectF(), frameAddedCallback);
}

bool GifRecorder::addFrame(const QRectF& rect, const FrameAddedCallback& frameAddedCallback)
{
    const QRect frameRect = mFrameNumber == 0 ? mFullFrameRect : rect.toRect();
    calcFramePosition(frameRect);

    const bool grabbed = mSceneGrabber.grabScene(frameRect,
        [this, frameAddedCallback](QImage&& img){
            mFrame = std::make_unique<QImage>(std::forward<QImage>(img));
            runRecordFrameThread([frameAddedCallback]{ if (frameAddedCallback) frameAddedCallback(); });
        });

    if (!grabbed)
    {
        qDebug() << "Could not grab frame";
        return false;
    }

    ++mFrameNumber;
    return true;
}

void GifRecorder::setFrameDuration(FrameRate frameRate)
{
    switch (frameRate)
    {
    case FPS_25:
        mFrameDuration = 4;
        break;
    case FPS_10:
        mFrameDuration = 10;
        break;
    case FPS_2:
        mFrameDuration = 50;
        break;
    case FPS_1:
        mFrameDuration = 100;
        break;
    }

    qDebug() << "Frame duration:" << mFrameDuration;
}

void GifRecorder::calcFramePosition(const QRectF& frameRect)
{
    mFramePosition = frameRect.translated(-mFullFrameRect.topLeft()).toRect().topLeft();
}

void GifRecorder::recordFrame()
{
    Q_ASSERT(mGifEncoder);
    Q_ASSERT(mFrame);
    const uint8_t* frame = mFrame->constBits();

    mGifEncoder->push(frame, mFramePosition.x(), mFramePosition.y(),
                      mFrame->width(), mFrame->height(), mFrameDuration);
}

void GifRecorder::runRecordFrameThread(const std::function<void()>& whenFinished)
{
    QThread* thread = QThread::create([this]{ recordFrame(); });
    mRecordingThread.reset(thread);
    QObject::connect(thread, &QThread::finished, this, whenFinished, Qt::SingleShotConnection);
    mRecordingThread->start();
}

}

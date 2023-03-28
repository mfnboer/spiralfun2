// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include "gif_recorder.h"
#include "mutation.h"
#include "scene_grabber.h"
#include <QVariant>
#include <vector>

namespace SpiralFun {

class ISequencePlayer
{
public:
    virtual ~ISequencePlayer() = default;
    virtual int getMaxDiameter() const = 0;
    virtual const QRectF& getSceneRect() const = 0;
    virtual QRectF getMaxSceneRect() const = 0;
    virtual std::unique_ptr<SceneGrabber> createSceneGrabber(const QRectF& rect) = 0;
    virtual void playSequence() = 0;

    using SavedCallback = std::function<void(bool success)>;
    virtual bool saveImage(const QRectF cutRect, const QString subDir, const QString& baseNameSuffix,
                           const ISequencePlayer::SavedCallback& savedCallback) = 0;
};

class MutationSequence : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum SaveAs { SAVE_AS_NONE, SAVE_AS_PICS, SAVE_AS_GIF };
    Q_ENUM(SaveAs);

    MutationSequence(const CircleList& circles, ISequencePlayer& sequencePlayer);
    ~MutationSequence();

    void setSequenceLength(int sequenceLength) { mSequenceLength = sequenceLength; }
    void setMutations(const QVariant& mutationsQmlList);
    void setCreateNewPicturesFolder(bool create) { mCreateNewPictureFolder = create; }
    void setFrameRate(GifRecorder::FrameRate frameRate) { mFrameRate = frameRate; }
    int getCurrentSequenceFrame() const { return mCurrentSequenceFrame; }
    int getSequenceLength() const { return mSequenceLength; }
    void play(SaveAs saveAs);

signals:
    void sequenceFramePlaying(int frame);
    void sequenceFinished();

private:
    struct CircleTraits
    {
        int mDiameter;
        int mSpeed;
    };

    void backupCircleSettings();
    void restoreCircleSettings();
    void playNextFrame();
    void postFrameProcessing();
    bool preparePlay();
    bool setupGifRecording();

    int mSequenceLength = 10;
    int mCurrentSequenceFrame = 0;
    std::vector<Mutation*> mMutations;
    std::vector<CircleTraits> mOrigCircleSettings;
    SaveAs mSaveAs = SAVE_AS_NONE;
    bool mCreateNewPictureFolder = true;
    GifRecorder::FrameRate mFrameRate = GifRecorder::FPS_10;
    QString mPicturesSubDir;
    const CircleList& mCircles;
    ISequencePlayer& mSequencePlayer;
    std::unique_ptr<SceneGrabber> mSceneGrabber;
    std::unique_ptr<GifRecorder> mGifRecorder;
    const QRectF mMaxSceneRect;
    QRectF mPreviousFrameRect;
};

}

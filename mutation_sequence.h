// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include "recorder.h"
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
    virtual QRectF getBoundingRect() const = 0;
    virtual std::unique_ptr<SceneGrabber> createSceneGrabber(const QRectF& rect) = 0;
    virtual void playSequenceFrame() = 0;

    using SavedCallback = std::function<void(bool success)>;
    virtual bool saveImage(const QRectF cutRect, const QString subDir, const QString& baseNameSuffix,
                           const ISequencePlayer::SavedCallback& savedCallback) = 0;
};

class MutationSequence : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum SaveAs { SAVE_AS_NONE, SAVE_AS_PICS, SAVE_AS_GIF, SAVE_AS_VIDEO };
    Q_ENUM(SaveAs);

    static bool isVideoType(SaveAs saveAs);

    MutationSequence() = default; // Needed for QML_ELEMENT
    MutationSequence(const CircleList* circles, ISequencePlayer* sequencePlayer);
    ~MutationSequence();

    void setCircles(const CircleList* circles) { mCircles = circles; }
    void setSequencePlayer(ISequencePlayer* sequencePlayer) { mSequencePlayer = sequencePlayer; }
    void setSequenceLength(int sequenceLength) { mSequenceLength = sequenceLength; }
    void setAddReverseSequence(int addReverse) { mAddReverseSequence = addReverse; }
    void setMutations(const QVariant& mutationsQmlList);
    void setCreateNewPicturesFolder(bool create) { mCreateNewPictureFolder = create; }
    void setFrameRate(Recorder::FrameRate frameRate) { mFrameRate = frameRate; }
    int getCurrentSequenceFrame() const { return mCurrentSequenceFrame; }
    int getTotalSequenceLength() const { return mAddReverseSequence ? mSequenceLength * 2 - 1 : mSequenceLength; }
    void play(SaveAs saveAs);

signals:
    void sequenceFramePlaying(int frame);
    void sequenceFinished(bool success);

private:
    struct CircleTraits
    {
        int mDiameter;
        int mSpeed;
    };

    void calcMaxSceneRect();
    void backupCircleSettings();
    void restoreCircleSettings();
    void playNextFrame();
    void playMutation(unsigned index, bool reverse = false);
    void postFrameProcessing();
    bool preparePlay();
    bool setupRecording(Recorder::Format format);

    int mSequenceLength = 10;
    int mCurrentSequenceFrame = 0;
    std::vector<Mutation*> mMutations;
    std::vector<CircleTraits> mOrigCircleSettings;
    SaveAs mSaveAs = SAVE_AS_NONE;
    bool mCreateNewPictureFolder = true;
    Recorder::FrameRate mFrameRate = Recorder::FPS_10;
    bool mAddReverseSequence = false;
    QString mPicturesSubDir;
    const CircleList* mCircles = nullptr;
    ISequencePlayer* mSequencePlayer = nullptr;
    std::unique_ptr<Recorder> mRecorder;
    QRectF mMaxSceneRect;
    QRectF mPreviousFrameRect;
};

}

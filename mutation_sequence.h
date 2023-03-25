// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include "mutation.h"
#include <QVariant>
#include <vector>

namespace SpiralFun {

class ISequencePlayer
{
public:
    virtual ~ISequencePlayer() = default;
    virtual void playSequence() = 0;
};

class MutationSequence : public QObject
{
    Q_OBJECT

public:
    MutationSequence(const CircleList& circles, ISequencePlayer& sequencePlayer);

    void setSequenceLength(int sequenceLength) { mSequenceLength = sequenceLength; }
    void setMutations(const QVariant& mutationsQmlList);
    int getCurrentSequenceFrame() const { return mCurrentSequenceFrame; }
    int getSequenceLength() const { return mSequenceLength; }
    void play();

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

    int mSequenceLength = 10;
    int mCurrentSequenceFrame = 0;
    std::vector<Mutation*> mMutations;
    std::vector<CircleTraits> mOrigCircleSettings;
    const CircleList& mCircles;
    ISequencePlayer& mSequencePlayer;
};

}

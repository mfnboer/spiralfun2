// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "mutation_sequence.h"
#include "spiral_scene.h"
#include "utils.h"

namespace SpiralFun {

MutationSequence::MutationSequence(const CircleList& circles, ISequencePlayer& sequencePlayer) :
    mCircles(circles),
    mSequencePlayer(sequencePlayer)
{
}

MutationSequence::~MutationSequence()
{
    if (mOrigCircleSettings.size() == mCircles.size())
        restoreCircleSettings();
}

void MutationSequence::setMutations(const QVariant& mutationsQmlList)
{
    const auto mutationList = mutationsQmlList.value<QQmlListReference>();
    mMutations.clear();
    mMutations.reserve(mutationList.size());

    for (int i = 0; i < mutationList.size(); ++i)
    {
        Mutation* mutation = dynamic_cast<Mutation*>(mutationList.at(i));
        mutation->init(mCircles);
        mMutations.push_back(mutation);
    }
}

void MutationSequence::backupCircleSettings()
{
    mOrigCircleSettings.clear();
    mOrigCircleSettings.reserve(mCircles.size());

    for (const auto& c : mCircles)
    {
        CircleTraits traits;
        traits.mDiameter = c->getDiameter();
        traits.mSpeed = c->getSpeed();
        mOrigCircleSettings.push_back(traits);
    }

    qDebug() << "Circle settings backed up";
}

void MutationSequence::restoreCircleSettings()
{
    Q_ASSERT(mOrigCircleSettings.size() == mCircles.size());
    for (unsigned i = 0; i < mOrigCircleSettings.size(); ++i)
    {
        auto& c = mCircles[i];
        const auto& traits = mOrigCircleSettings[i];
        c->setDiameter(traits.mDiameter);
        c->setSpeed(traits.mSpeed);
    }

    mOrigCircleSettings.clear();
    qDebug() << "Circle settings restored";
}

void MutationSequence::play(SaveAs saveAs)
{
    mSaveAs = saveAs;

    if (!preparePlay())
    {
        emit sequenceFinished();
        return;
    }

    // Unfortunately an interface cannot have signals
    auto* hack = dynamic_cast<SpiralScene*>(&mSequencePlayer);
    Q_ASSERT(hack);
    connect(hack, &SpiralScene::sequenceFramePlayed, this, [this]{ postFrameProcessing(); });
    mCurrentSequenceFrame = 0;
    emit sequenceFramePlaying(mCurrentSequenceFrame);
    mSequencePlayer.playSequence();
}

void MutationSequence::playNextFrame()
{
    if (++mCurrentSequenceFrame < mSequenceLength && !mMutations.empty())
    {
        const unsigned index = (mCurrentSequenceFrame - 1) % mMutations.size();
        const auto* mutation = mMutations[index];
        qDebug() << mutation->getCircle() << mutation->getTrait() << mutation->getChange();
        mutation->apply(mCircles, mSequencePlayer.getMaxDiameter());
        emit sequenceFramePlaying(mCurrentSequenceFrame);
        mSequencePlayer.playSequence();
    }
    else
    {
        qDebug() << "Finished playing mutation sequence";

        if (mSaveAs == SAVE_AS_GIF)
            mGifRecorder->stopRecording(true);

        restoreCircleSettings();
        emit sequenceFinished();
    }
}

void MutationSequence::postFrameProcessing()
{
    switch (mSaveAs)
    {
    case SAVE_AS_NONE:
        playNextFrame();
        break;
    case SAVE_AS_PICS: {
        const QString suffix = QString("_MS%1").arg(mCurrentSequenceFrame + 1, 3, 10, QChar('0'));
        const QRectF frameRect = mSequencePlayer.getMaxSceneRect();
        mSequencePlayer.saveImage(frameRect, mPicturesSubDir, suffix, [this](bool){ playNextFrame(); });
        break; }
    case SAVE_AS_GIF: {
        mGifRecorder->addFrame([this]{ playNextFrame(); });
        break; }
    }
}

bool MutationSequence::preparePlay()
{
    backupCircleSettings();

    if (mSaveAs == SAVE_AS_GIF && !setupGifRecording())
    {
        qDebug() << "Failed to setup GIF recording";
        return false;
    }

    if (mCreateNewPictureFolder)
    {
        const QString dateTimeName = Utils::createDateTimeName();
        mPicturesSubDir = QString("SpiralFun_%1").arg(dateTimeName);
    }
    else
    {
        mPicturesSubDir.clear();
    }

    return true;
}

bool MutationSequence::setupGifRecording()
{
    const auto rect = mSequencePlayer.getMaxSceneRect();
    mSceneGrabber = mSequencePlayer.createSceneGrabber(rect);
    mGifRecorder = std::make_unique<GifRecorder>(*mSceneGrabber);

    return mGifRecorder->startRecording(mFrameRate, "_MS");
}

}

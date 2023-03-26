// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "mutation_sequence.h"
#include "spiral_scene.h"

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

void MutationSequence::play()
{
    backupCircleSettings();

    // Unfortunately an interface cannot have signals
    auto* hack = dynamic_cast<SpiralScene*>(&mSequencePlayer);
    Q_ASSERT(hack);
    connect(hack, &SpiralScene::sequenceFramePlayed, this, [this]{ playNextFrame(); });
    mCurrentSequenceFrame = 0;
    mSequencePlayer.playSequence();
    emit sequenceFramePlaying(mCurrentSequenceFrame);
}

void MutationSequence::playNextFrame()
{
    if (++mCurrentSequenceFrame < mSequenceLength && !mMutations.empty())
    {
        const unsigned index = (mCurrentSequenceFrame - 1) % mMutations.size();
        const auto* mutation = mMutations[index];
        qDebug() << mutation->getCircle() << mutation->getTrait() << mutation->getChange();
        mutation->apply(mCircles, mSequencePlayer.getMaxDiameter());
        mSequencePlayer.playSequence();
        emit sequenceFramePlaying(mCurrentSequenceFrame);
    }
    else
    {
        qDebug() << "Finished playing mutation sequence";
        restoreCircleSettings();
        emit sequenceFinished();
    }
}

}

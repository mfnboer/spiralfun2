// Copyright (C) 2024 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include <QSoundEffect>
#include <QObject>
#include <queue>

namespace SpiralFun {

class SpiralScene;

class MusicGenerator : public QObject
{
    Q_OBJECT

public:
    MusicGenerator(const CircleList &circles, qreal toneDistance, int tonePlayInterval, SpiralScene* scene);

    int getTonePlayInterval() const { return mTonePlayInterval; }
    void playNotes();

private:
    void initNotes();
    void playNote(const std::unique_ptr<SpiralFun::Circle>& circle);

    const CircleList& mCircles;

    struct Sound
    {
        int mIndex;
        std::unique_ptr<QSoundEffect> mSound;
    };

    std::vector<QString> mNotes;
    qreal mNoteSize = 1.0;
    std::unordered_map<Circle*, std::queue<Sound>> mSounds;
    qreal mToneDistance = 25.0;
    int mTonePlayInterval = 1;
    SpiralScene* mScene;
};

}

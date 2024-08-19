// Copyright (C) 2024 Michel de Boer
// License: GPLv3
#include "music_generator.h"
#include "flash.h"
#include "spiral_scene.h"
#include <QAudioDevice>
#include <QMediaDevices>
#include <ranges>

namespace SpiralFun {

static const std::initializer_list<QString> NOTES = { "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B" };

MusicGenerator::MusicGenerator(const CircleList &circles, qreal toneDistance, int tonePlayInterval, SpiralScene* scene) :
    mCircles(circles),
    mToneDistance(toneDistance),
    mTonePlayInterval(tonePlayInterval),
    mScene(scene)
{
    Q_ASSERT(scene);
    initNotes();
}

void MusicGenerator::initNotes()
{
    for (int i = 2; i <= 5; ++i)
    {
        for (const auto& note : NOTES)
        {
            const QString noteFile = QString(":/piano-wav/%1%2.wav").arg(note).arg(i);
            mNotes.push_back(noteFile);
        }
    }

    mNoteSize = SpiralScene::MAX_DIAMETER / 2.0 / mNotes.size();
}

void MusicGenerator::playNotes()
{
    bool draw = false;

    for (auto& circle : std::ranges::reverse_view(mCircles))
    {
        if (!circle->getDraw())
            continue;

        // The first circle with draw enabled determines when to play tones.
        if (!draw && circle->getDrawnLength() < mToneDistance && !mSounds[circle.get()].empty())
            return;

        draw = true;
        playNote(circle);
    }
}

void MusicGenerator::playNote(const std::unique_ptr<SpiralFun::Circle>& circle)
{
    const qreal MAX_DISTANCE = SpiralScene::MAX_DIAMETER / 2.0;
    QLineF line(mCircles[0]->getCenter(), circle->getCenter());
    const auto distance = std::min(line.length(), MAX_DISTANCE);
    int noteIndex = std::min((int)(distance / mNoteSize), (int)mNotes.size() - 1);
    auto& playing = mSounds[circle.get()];

    if (!playing.empty())
    {
        if (noteIndex == playing.back().mIndex)
        {
            if (noteIndex + NOTES.size() < mNotes.size())
                noteIndex += NOTES.size();
            else
                noteIndex -= NOTES.size();
        }
    }

    const QString& newNote = mNotes.at(noteIndex);
    auto sound = std::make_unique<QSoundEffect>(QMediaDevices::defaultAudioOutput(), this);
    sound->setSource(QUrl::fromLocalFile(newNote));
    sound->setVolume(1.0);
    sound->play();

    playing.push(Sound{ noteIndex, std::move(sound) });

    if (playing.size() > 3)
        playing.pop();

    qDebug() << "Play:" << newNote;
    circle->setDrawnLength(0.0);

    new Flash(circle->getCenter(), circle->getColor(), mScene);
}

}

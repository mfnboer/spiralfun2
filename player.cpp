// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "player.h"
#include "spiral_scene.h"
#include <QAudioDevice>
#include <QFile>
#include <QMediaDevices>
#include <QTime>
#include <chrono>
#include <ranges>

using namespace std::chrono_literals;

//static const std::vector<QString> NOTES = { "c", "cs", "d", "ds", "e", "f", "fs", "g", "gs", "a", "as", "b" };
static const std::vector<QString> NOTES = { "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B" };

namespace SpiralFun {

Player::Player(const CircleList &circles, bool musicGeneration, qreal toneDistance, int tonePlayInterval) :
    mCircles(circles),
    mMusicGeneration(musicGeneration),
    mToneDistance(toneDistance),
    mTonePlayInterval(tonePlayInterval)
{
    if (mMusicGeneration)
        initNotes();

    mPlayTimer.setInterval(mMusicGeneration ? mTonePlayInterval : 0);
    QObject::connect(&mPlayTimer, &QTimer::timeout, this, &Player::advance);
    mSceneRefreshTimer.setInterval(40ms);
    QObject::connect(&mSceneRefreshTimer, &QTimer::timeout, this, [this]{ emit refreshScene(); });
}

Player::~Player()
{
    stopTimers();
}

void Player::initNotes()
{
    const auto devices = QMediaDevices::audioOutputs();
    for (const auto& dev : devices)
        qDebug() << "AUDIO:" << dev.description() << dev.maximumSampleRate();
    qDebug() << "AUDIO DFLT:" << QMediaDevices::defaultAudioOutput().description();

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

bool Player::play(std::unique_ptr<Recorder> recorder)
{
    for (auto& circle : mCircles)
        circle->preparePlay();

    mStepsPerInterval = 1;
    mStartTime = QTime::currentTime().msecsSinceStartOfDay();
    mCycles = 0;
    mRecorder = std::move(recorder);
    startTimers();

    if (mRecorder)
    {
        if (!setupRecording())
            return false;
    }

    return true;
}

void Player::playAll()
{
    for (auto& circle : mCircles)
        circle->preparePlay();

    mStepsPerInterval = 100;
    mStartTime = QTime::currentTime().msecsSinceStartOfDay();
    mCycles = 0;
    mPlayTimer.start();
}

void Player::startTimers()
{
    mPlayTimer.start();
    mSceneRefreshTimer.start();
}

void Player::stopTimers()
{
    mPlayTimer.stop();
    mSceneRefreshTimer.stop();
}

void Player::advance()
{
    ++mCycles;
    for (unsigned step = 0.0; step < mStepsPerInterval; ++step)
    {
        advanceCircles(mStepAngle);
        mAngle += mStepAngle;
        emit angleChanged();

        if (mAngle >= M_PI * 2)
        {
            finishPlaying();
            break;
        }

        if (mRecording)
        {
            if (!record())
                recordingFailed();
        }
    }

    if (mMusicGeneration)
        playNotes();
}

void Player::recordingFailed()
{
    stopTimers();
    Stats stats;
    stats.mRecordingFailed = true;
    emit done(stats);
    mRecorder = nullptr;
}

void Player::finishPlaying()
{
    stopTimers();
    const qreal t = (QTime::currentTime().msecsSinceStartOfDay() - mStartTime) / 1000.0;
    const qreal avgCycleT = t / mCycles;
    qInfo() << "Play duration:" << t << "secs" << "cycles:" << mCycles <<
                "avg-cycle-time:" << avgCycleT * 1000 << "ms";

    Stats stats;
    stats.mCycles = mCycles;
    stats.mPlayTime = std::lround(t * 1000) * 1ms;

    // Draw the last line to close the curve. It may not have been drawn yet
    // due to the minimum draw length.
    forceDraw();
    emit refreshScene();

    if (mRecording)
    {
        // Record last frame
        updateRecordingRect();

        const bool frameAdded = mRecorder->addFrame(mRecordingRect, [this, stats](bool frameAdded){
            if (!frameAdded)
                qWarning() << "Adding last frame failed";

            mRecorder->stopRecording(true);
                emit done(stats);
            });

        if (!frameAdded)
        {
            qWarning() << "Failed to add last frame";
            mRecorder->stopRecording(true);
            emit done(stats);
        }
    }
    else
    {
        emit done(stats);
    }
}

void Player::advanceCircles(qreal angle)
{
    for(unsigned i = 1; i < mCircles.size(); ++i)
        advanceCircle(i, angle);
}

void Player::advanceCircle(unsigned index, qreal angle)
{
    if (index < 1 || index >= mCircles.size())
        throw std::out_of_range(std::string("index=") + std::to_string(index));

    const QPointF& rotationCenter = mCircles[index - 1]->getCenter();
    const int speed = mCircles[index]->getSpeed();
    const bool clockwise = speed > 0;

    for (unsigned i = index; i < mCircles.size(); ++i)
    {
        auto& circle = *mCircles[i];
        for (int n = 0; n < std::abs(speed); ++n)
            circle.rotate(rotationCenter, angle, clockwise);
    }
}

void Player::forceDraw()
{
    for (auto& circle : mCircles)
    {
        if (circle->getDraw())
            circle->forceDrawToCenter();
    }
}

void Player::playNotes()
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

void Player::playNote(const std::unique_ptr<SpiralFun::Circle>& circle)
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
}

bool Player::setupRecording()
{
    Q_ASSERT(mRecorder);
    if (!mRecorder->startRecording(Recorder::FPS_25))
        return false;

    mFullFrameRect = mRecorder->getFullFrameRect();

    // Capture one full frame, next frames will be subframes where changes happened.
    mRecordingRect = mFullFrameRect;
    mRecordAngle = mRecordAngleThreshold;
    mRecording = true;

    return record();
}

void Player::resetRecordingRect()
{
    mRecordingRect = mRecorder->calcBoundingRectangle(mCircles) & mFullFrameRect;
}

void Player::updateRecordingRect()
{
    mRecordingRect |= mRecorder->calcBoundingRectangle(mCircles) & mFullFrameRect;
}

bool Player::record()
{
    updateRecordingRect();
    mRecordAngle += mStepAngle;

    if (mRecordAngle < mRecordAngleThreshold)
        return true;

    mRecordAngle = 0.0;
    const bool frameAdded = mRecorder->addFrame(mRecordingRect, [this](bool frameAdded){
        if (!frameAdded)
        {
            qWarning() << "Adding last frame failed";
            recordingFailed();
            return;
        }

        mPlayTimer.start();
    });

    if (!frameAdded)
    {
        qDebug() << "Could not add frame";
        return false;
    }

    resetRecordingRect();
    mPlayTimer.stop();
    return true;
}

}

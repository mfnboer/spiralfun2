// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include "mutation_sequence.h"
#include "player.h"
#include "scene_grabber.h"
#include "scoped_line.h"
#include "spiral_config.h"
#include <QQuickItem>
#include <QQuickWindow>
#include <memory>
#include <cstdint>
#include <unordered_map>

namespace SpiralFun {

class SpiralScene : public QQuickItem, public ISequencePlayer
{
    Q_OBJECT
    Q_PROPERTY(QString VERSION MEMBER VERSION CONSTANT)
    Q_PROPERTY(int MIN_CIRCLES MEMBER MIN_CIRCLES CONSTANT)
    Q_PROPERTY(int MAX_CIRCLES MEMBER MAX_CIRCLES CONSTANT)
    Q_PROPERTY(int MAX_DIAMETER MEMBER MAX_DIAMETER NOTIFY maxDiameterChanged)
    Q_PROPERTY(int MAX_ROTATIONS MEMBER MAX_ROTATIONS CONSTANT)
    Q_PROPERTY(int MAX_DRAW MEMBER MAX_DRAW CONSTANT)
    Q_PROPERTY(int CFG_IMAGE_SIZE MEMBER CFG_IMAGE_SIZE CONSTANT)
    Q_PROPERTY(int numCircles READ getNumCircles WRITE setNumCircles NOTIFY numCirclesChanged)
    Q_PROPERTY(SpiralFun::Circle* currentCircle READ getCurrentCircle NOTIFY currentCircleChanged)
    Q_PROPERTY(int currentCircleIndex READ getCurrentCircleIndex NOTIFY currentCircleIndexChanged)
    Q_PROPERTY(SpiralScene::PlayState playState READ getPlayState NOTIFY playStateChanged)
    Q_PROPERTY(qreal playAngle READ getPlayAngle NOTIFY playAngleChanged);
    Q_PROPERTY(int sequenceFrame READ getSequenceFrame NOTIFY sequenceFrameChanged)
    Q_PROPERTY(int sequenceLength READ getSequenceLength NOTIFY sequenceLengthChanged)
    Q_PROPERTY(SpiralScene::ShareMode shareMode READ getShareMode NOTIFY shareModeChanged)
    Q_PROPERTY(bool sharingInProgress MEMBER mSharingInProgress NOTIFY sharingInProgressChanged)
    QML_ELEMENT

public:
    enum PlayState { NOT_PLAYING, PLAYING, PLAYING_SEQUENCE, RECORDING, DONE_PLAYING };
    Q_ENUM(PlayState)

    enum ShareMode { SHARE_NONE, SHARE_PIC, SHARE_GIF, SHARE_VIDEO };
    Q_ENUM(ShareMode)

    struct Stats
    {
        Player::Stats mPlayerStats;
        uint32_t mLineSegmentCount = 0;
        uint32_t mLineCount = 0;
        uint32_t mLinePointsSum = 0;
    };

    explicit SpiralScene(QQuickItem *parent = nullptr);

    void setupCircles(const SpiralFun::CircleConfigList& config = DEFAULT_CONFIG);
    int getNumCircles() const { return mCircles.size(); }
    SpiralFun::Circle* getCurrentCircle() const;
    int getCurrentCircleIndex() const { return mCurrentIndex; }
    PlayState getPlayState() const { return mPlayState; }
    ShareMode getShareMode() const { return mShareMode; }
    int getMaxDiameter() const override { return MAX_DIAMETER; }
    qreal getPlayAngle() const { return mPlayer ? mPlayer->getAngle() : 0.0; }
    int getSequenceFrame() const { return mMutationSequence ? mMutationSequence->getCurrentSequenceFrame() : 0; }
    int getSequenceLength() const { return mMutationSequence ? mMutationSequence->getTotalSequenceLength() : 0; }
    void setNumCircles(int numCircles);
    void selectCircle(Circle* circle);
    ScopedLine addLine(QObject* object, const QColor& color, int lineWidth, const QPointF& startPoint);
    void playSequenceFrame() override;
    const QRectF& getSceneRect() const override { return mSceneRect; }
    QRectF getMaxSceneRect() const override;
    QRectF getBoundingRect() const override { return boundingRect(); }
    std::unique_ptr<SceneGrabber> createSceneGrabber(const QRectF& rect) override;

    Q_INVOKABLE void init();
    Q_INVOKABLE void setupExample(const QString& example);
    Q_INVOKABLE void circleUp();
    Q_INVOKABLE void circleDown();
    Q_INVOKABLE bool checkPlayRequirement();
    Q_INVOKABLE void play();
    Q_INVOKABLE void playSequence(const QVariant& mutations, int sequenceLength, bool addReverse,
                                  MutationSequence::SaveAs saveAs, bool createAlbum,
                                  Recorder::FrameRate frameRate);
    Q_INVOKABLE void showSpiralStats();
    Q_INVOKABLE void record(Recorder::Format format);
    Q_INVOKABLE void stop();
    Q_INVOKABLE bool saveImage(const QRectF cutRect = {}, const QString subDir = "", const QString& baseNameSuffix = "",
                               const ISequencePlayer::SavedCallback& savedCallback = nullptr) override;
    Q_INVOKABLE void saveConfig();
    Q_INVOKABLE void share();
    Q_INVOKABLE QObjectList getConfigFileList();
    Q_INVOKABLE void loadConfig(const QString& fileName);
    Q_INVOKABLE void deleteConfig(const QStringList& fileNameList);
    Q_INVOKABLE qreal getDevicePixelRatio() const { return window()->devicePixelRatio(); }
    Q_INVOKABLE QStringList getCircleColorList() const;
    Q_INVOKABLE bool sendAppToBackground();

signals:
    void maxDiameterChanged();
    void currentCircleChanged();
    void currentCircleIndexChanged();
    void numCirclesChanged();
    void playStateChanged();
    void playAngleChanged();
    void sequenceFrameChanged();
    void sequenceLengthChanged();
    void shareModeChanged();
    void sharingInProgressChanged();
    void message(const QString message);
    void statusUpdate(const QString message);
    void sequenceFramePlayed();

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;
    void touchEvent(QTouchEvent* event) override;

private:
    void calcDefaultRadiusSize();
    void handleWindowSizeChanged();
    SpiralFun::Circle* addCircle(qreal radius);
    std::optional<unsigned> findCircle(const Circle* circle);
    void handleDiameterChange(Circle* circle, int oldDiameter);
    void moveCircles(unsigned index, qreal yShift);
    void setCurrentCircleFocus(bool focus);
    void setCurrentIndex(unsigned index);
    void addCirclesToScene();
    void removeCirclesFromScene();
    void resetCircles();
    void resetScene();
    void handleMediaScannerFinished(const QString& contentUri);
    void setSharingInProgress(bool inProgress);
    void handleReceivedAndroidIntent(const QString& uri);
    void setPlayState(PlayState state);
    void setShareMode(ShareMode shareMode);
    QSGNode* createLineNode(Line& line);
    void updateSceneRect(const QPointF& p);
    void doPlay(std::unique_ptr<Recorder> recorder);
    void shareImage();
    void shareMedia();
    bool hasVideoTypeShareMode() const;
    void shareVideo();

    std::unordered_map<QObject*, Line> mLines;
    bool mDoRender = true;
    bool mClearScene = false;
    QRectF mSceneRect;
    Stats mStats;
    CircleList mCircles;
    qreal mDefaultCircleRadius = 10.0;
    unsigned mCurrentIndex = 0;
    std::unique_ptr<Player> mPlayer;
    PlayState mPlayState = NOT_PLAYING;
    qreal mScaleFactor = 1.0;
    std::vector<std::unique_ptr<QObject>> mConfigFileList;
    bool mSharingInProgress = false;
    ShareMode mShareMode = SHARE_NONE;
    QString mShareMediaUri;
    std::unique_ptr<SceneGrabber> mSceneGrabber;
    std::unique_ptr<MutationSequence> mMutationSequence;

    static constexpr int MIN_CIRCLES = SpiralConfig::MIN_CIRCLES;
    static constexpr int MAX_CIRCLES = SpiralConfig::MAX_CIRCLES;
    int MAX_DIAMETER = 300;
    static constexpr int MAX_ROTATIONS = SpiralConfig::MAX_SPEED;
    static constexpr int MAX_DRAW = Circle::MAX_DRAW;
    static constexpr int CFG_IMAGE_SIZE = 150;
    static constexpr const char* VERSION = APP_VERSION;
};

}

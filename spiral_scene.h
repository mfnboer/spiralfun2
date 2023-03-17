// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include "player.h"
#include "scene_grabber.h"
#include "scoped_line.h"
#include "spiral_config.h"
#include <QQuickItem>
#include <memory>
#include <cstdint>
#include <unordered_map>

namespace SpiralFun {

class SpiralScene : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString VERSION MEMBER VERSION CONSTANT)
    Q_PROPERTY(int MIN_CIRCLES MEMBER MIN_CIRCLES CONSTANT)
    Q_PROPERTY(int MAX_CIRCLES MEMBER MAX_CIRCLES CONSTANT)
    Q_PROPERTY(int MAX_DIAMETER MEMBER MAX_DIAMETER NOTIFY maxDiameterChanged)
    Q_PROPERTY(int MAX_ROTATIONS MEMBER MAX_ROTATIONS CONSTANT)
    Q_PROPERTY(int CFG_IMAGE_SIZE MEMBER CFG_IMAGE_SIZE CONSTANT)
    Q_PROPERTY(int numCircles READ getNumCircles WRITE setNumCircles NOTIFY numCirclesChanged)
    Q_PROPERTY(SpiralFun::Circle* currentCircle READ getCurrentCircle NOTIFY currentCircleChanged)
    Q_PROPERTY(int currentCircleIndex READ getCurrentCircleIndex NOTIFY currentCircleIndexChanged)
    Q_PROPERTY(SpiralScene::PlayState playState READ getPlayState NOTIFY playStateChanged)
    Q_PROPERTY(SpiralScene::ShareMode shareMode READ getShareMode NOTIFY shareModeChanged)
    Q_PROPERTY(bool sharingInProgress MEMBER mSharingInProgress NOTIFY sharingInProgressChanged)
    QML_ELEMENT

public:
    enum PlayState { NOT_PLAYING, PLAYING, RECORDING, DONE_PLAYING };
    Q_ENUM(PlayState)

    enum ShareMode { SHARE_PIC, SHARE_VID };
    Q_ENUM(ShareMode)

    explicit SpiralScene(QQuickItem *parent = nullptr);

    void setupCircles(const SpiralFun::CircleConfigList& config = DEFAULT_CONFIG);
    int getNumCircles() const { return mCircles.size(); }
    SpiralFun::Circle* getCurrentCircle() const;
    int getCurrentCircleIndex() const { return mCurrentIndex; }
    PlayState getPlayState() const { return mPlayState; }
    ShareMode getShareMode() const { return mShareMode; }
    void setNumCircles(int numCircles);
    void selectCircle(Circle* circle);
    ScopedLine addLine(QObject* object, const QColor& color, const QPointF& startPoint);

public slots:
    void init();
    void setupExample(const QString& example);
    void circleUp();
    void circleDown();
    void play();
    void record();
    void stop();
    bool saveImage();
    void saveConfig();
    void share();
    QObjectList getConfigFileList();
    void loadConfig(const QString& fileName);
    void deleteConfig(const QStringList& fileNameList);

signals:
    void maxDiameterChanged();
    void currentCircleChanged();
    void currentCircleIndexChanged();
    void numCirclesChanged();
    void playStateChanged();
    void shareModeChanged();
    void sharingInProgressChanged();
    void message(const QString message);

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
    QSGNode* createLineNode(const Line& line);
    void updateSceneRect(const QPointF& p);
    void doPlay(std::unique_ptr<SceneGrabber> recorder);
    void shareImage();
    void shareVideo();

    std::unordered_map<QObject*, Line> mLines;
    bool mClearScene = false;
    QRectF mSceneRect;
    uint64_t mLineSegmentCount = 0;
    CircleList mCircles;
    qreal mDefaultCircleRadius = 10.0;
    unsigned mCurrentIndex = 0;
    std::unique_ptr<Player> mPlayer;
    PlayState mPlayState = NOT_PLAYING;
    qreal mScaleFactor = 1.0;
    std::vector<std::unique_ptr<QObject>> mConfigFileList;
    bool mSharingInProgress = false;
    ShareMode mShareMode = SHARE_PIC;
    QString mShareContentUri;
    std::unique_ptr<SceneGrabber> mSceneGrabber;

    static constexpr int MIN_CIRCLES = SpiralConfig::MIN_CIRCLES;
    static constexpr int MAX_CIRCLES = SpiralConfig::MAX_CIRCLES;
    int MAX_DIAMETER = 300;
    static constexpr int MAX_ROTATIONS = SpiralConfig::MAX_SPEED;
    static constexpr int CFG_IMAGE_SIZE = 150;
    static constexpr const char* VERSION = APP_VERSION;
};

}

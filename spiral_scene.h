// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include "player.h"
#include <QQuickItem>
#include <memory>
#include <unordered_map>
#include <vector>

namespace SpiralFun {

struct CircleConfig
{
    qreal mRelRadius;
    int mSpeed;
    bool mDraw;
    QColor mColor;
};

const std::initializer_list<CircleConfig> DEFAULT_CONFIG = {
    { 4.0, 0, false, Qt::blue },
    { 2.0, 1, false, Qt::green },
    { 0.2, -5, true, Qt::white }
};

class SpiralScene : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int MIN_CIRCLES MEMBER MIN_CIRCLES CONSTANT)
    Q_PROPERTY(int MAX_CIRCLES MEMBER MAX_CIRCLES CONSTANT)
    Q_PROPERTY(int MAX_DIAMETER MEMBER MAX_DIAMETER CONSTANT)
    Q_PROPERTY(int MAX_ROTATIONS MEMBER MAX_ROTATIONS CONSTANT)
    Q_PROPERTY(int numCircles READ getNumCircles WRITE setNumCircles NOTIFY numCirclesChanged)
    Q_PROPERTY(SpiralFun::Circle* currentCircle READ getCurrentCircle NOTIFY currentCircleChanged)
    Q_PROPERTY(int currentCircleIndex READ getCurrentCircleIndex NOTIFY currentCircleIndexChanged)
    Q_PROPERTY(SpiralScene::PlayState playState READ getPlayState NOTIFY playStateChanged)
    QML_ELEMENT

public:
    enum PlayState { NOT_PLAYING = 0, PLAYING = 1, DONE_PLAYING = 2 };
    Q_ENUM(PlayState)

    struct Line
    {
          std::vector<QPointF> mLinePoints;
          QColor mColor;
          QSGNode* mRoot = nullptr;
    };

    SpiralScene(QQuickItem *parent = nullptr);

    int getNumCircles() const { return mCircles.size(); }
    SpiralFun::Circle* getCurrentCircle() const;
    int getCurrentCircleIndex() const { return mCurrentIndex; }
    PlayState getPlayState() const { return mPlayState; }

    void setNumCircles(int numCircles);

    void addLine(QObject* object, const QColor& color, const QPointF& startPoint);
    void removeLine(QObject* object);
    void addPoint(QObject* object, const QPointF& point);

public slots:
    void init();
    void setupCircles(const std::vector<CircleConfig>& config = DEFAULT_CONFIG);
    void circleUp();
    void circleDown();
    void play();
    void stop();

signals:
    void currentCircleChanged();
    void currentCircleIndexChanged();
    void numCirclesChanged();
    void playStateChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;

private:
    SpiralFun::Circle* addCircle(qreal radius);
    std::optional<unsigned> findCircle(const Circle* circle);
    void handleDiameterChange(Circle* circle, int oldDiameter);
    void moveCircles(unsigned index, qreal yShift);
    void setCurrentCircleFocus(bool focus);
    void setCurrentIndex(unsigned index);
    void addCirclesToScene(bool addLines = false);
    void removeCirclesFromScene(bool removeLines = false);
    void resetCircles();
    void resetScene();
    void setPlayState(PlayState state);

    QSGNode* createLineNode(const Line& line) const;
    std::unordered_map<QObject*, Line> mLines;
    bool mClearScene = false;
    CircleList mCircles;
    qreal mDefaultCircleRadius = 10.0;
    unsigned mCurrentIndex = 0;
    std::unique_ptr<Player> mPlayer;
    PlayState mPlayState = NOT_PLAYING;

    static constexpr int MIN_CIRCLES = 2;
    static constexpr int MAX_CIRCLES = 10;
    static constexpr int MAX_DIAMETER = 300;
    static constexpr int MAX_ROTATIONS = 9999;
};

}

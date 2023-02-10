// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include "color_select_button.h"
#include "graphics_view.h"
#include "player.h"
#include "spinbox.h"
#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsScene>
#include <QPushButton>
#include <QSpinBox>
#include <memory>
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

class AppWindow : public QWidget
{
    Q_OBJECT

public:
    AppWindow();

    void init();
    void setupCircles(const std::vector<CircleConfig>& config = DEFAULT_CONFIG);
    void advanceCircles(qreal angle);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    SpiralFun::Circle* addCircle(qreal radius);
    void forceDraw();
    void advanceCircle(unsigned index, qreal angle);
    void handleNumCircles(unsigned numCircles);
    void handleDiameter(unsigned diameter);
    void moveCircles(unsigned index, qreal yShift);
    void handleUp();
    void handleDown();
    void handleCircleSelect();
    void handleColor(const QColor& color);
    void handleDraw(bool draw);
    void handleRotations(unsigned rotations);
    void handleDirection(bool clockwise);
    void handlePlay();
    void handleStop();
    void setCurrentCircleFocus(bool focus);
    void enableControls(bool enable);
    void addCirclesToScene();
    void removeCirclesFromScene();
    void resetCircles();
    void resetScene();
    void examples();
    void helpInfo();
    void aboutInfo();

    GraphicsView* mView;
    QGraphicsScene* mScene;
    QPushButton* mUpButton;
    QPushButton* mDownButton;
    SpinBox* mDiameterSpinBox;
    QCheckBox* mDrawCheckBox;
    ColorSelectButton* mDrawColorButton;
    SpinBox* mRotationsSpinBox;
    QComboBox* mDirectionComboBox;
    SpinBox* mNumCirclesSpinBox;
    QPushButton* mStartStopButton;
    QPushButton* mMoreButton;
    std::vector<std::unique_ptr<SpiralFun::Circle>> mCircles;
    unsigned mCurrentIndex = 0;
    qreal mDefaultCircleRadius = 10.0;
    std::unique_ptr<Player> mPlayer;
};

}

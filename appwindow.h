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

class AppWindow : public QWidget
{
    Q_OBJECT

public:
    AppWindow();

    void init();
    void setupCircles();
    SpiralFun::Circle* addCircle(qreal radius);
    void advanceCircles(qreal angle);
    void addCirclesToScene();
    void removeCirclesFromScene();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void advanceCircle(unsigned index, qreal angle);
    void handleNumCircles(unsigned numCircles);
    void handleDiameter(unsigned diameter);
    void moveCircles(unsigned index, qreal yShift);
    void handleUp();
    void handleDown();
    void handleColor(const QColor& color);
    void handleDraw(bool draw);
    void handleRotations(unsigned rotations);
    void handleDirection(bool clockwise);
    void handlePlay();
    void handleStop();
    void setCurrentCircleFocus(bool focus);
    void enableControls(bool enable);
    void resetCircles();
    void resetScene();

    GraphicsView* mView;
    QGraphicsScene* mScene;
    SpinBox* mDiameterSpinBox;
    QCheckBox* mDrawCheckBox;
    ColorSelectButton* mDrawColorButton;
    SpinBox* mRotationsSpinBox;
    QComboBox* mDirectionComboBox;
    SpinBox* mNumCirclesSpinBox;
    QPushButton* mStartStopButton;
    std::vector<std::unique_ptr<SpiralFun::Circle>> mCircles;
    unsigned mCurrentIndex = 0;
    qreal mDefaultCircleRadius = 10.0;
    std::unique_ptr<Player> mPlayer;
};

}

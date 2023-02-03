#pragma once

#include "circle.h"
#include "player.h"
#include <QCheckBox>
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

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void advanceCircle(unsigned index, qreal angle);
    void handleNumCircles(unsigned numCircles);
    void handleDiameter(unsigned diameter);
    void moveCircles(unsigned index, qreal yShift);
    void handleUp();
    void handleDown();
    void handleDraw(bool draw);
    void handleRotations(unsigned rotations);
    void handleDirection(bool clockwise);
    void handlePlay();
    void handleStop();
    void setCurrentCircleFocus(bool focus);
    void enableControls(bool enable);
    void resetCircles();
    void resetScene();

    QGraphicsView* mView;
    QGraphicsScene* mScene;
    QSpinBox* mDiameterSpinBox;
    QCheckBox* mDrawCheckBox;
    QSpinBox* mRotationsSpinBox;
    QCheckBox* mDirectionCheckBox;
    QSpinBox* mNumCirclesSpinBox;
    QPushButton* mStartStopButton;
    std::vector<std::unique_ptr<SpiralFun::Circle>> mCircles;
    unsigned mCurrentIndex = 0;
    qreal mDefaultCircleRadius = 10.0;
    std::unique_ptr<Player> mPlayer;
};

}

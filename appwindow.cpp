#include "appwindow.h"
#include <QBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QStyle>
#include <QSpinBox>

namespace SpiralFun {

AppWindow::AppWindow()
{
    mView = new QGraphicsView();
    mView->setRenderHint(QPainter::Antialiasing);
    mView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    const QScreen* scr = screen();
    const QSize& screenSize = scr->size();
    qInfo() << "screen:" << screenSize << "dpr:" << scr->devicePixelRatio();

    mScene = new QGraphicsScene();
    mScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    const qreal width = screenSize.width();
    const qreal height = screenSize.height();
    mScene->setSceneRect(-width / 2.0, -height / 2.0, width, height);
    mView->setScene(mScene);

    auto* upButton = new QPushButton(style()->standardIcon(QStyle::SP_ArrowUp), "");
    upButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    auto* diameterLabel = new QLabel("Diameter:");
    auto* diameterSpinBox = new QSpinBox();
    diameterSpinBox->setMinimum(1);
    diameterSpinBox->setMaximum(300);
    auto* drawCheckBox = new QCheckBox("draw");
    auto* row1Layout = new QHBoxLayout();
    row1Layout->addWidget(upButton);
    row1Layout->addWidget(diameterLabel);
    row1Layout->addWidget(diameterSpinBox, 1);
    row1Layout->addWidget(drawCheckBox, 1);

    auto* row2Layout = new QHBoxLayout();
    auto* downButton = new QPushButton(style()->standardIcon(QStyle::SP_ArrowDown), "");
    downButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    auto* rotationsLabel = new QLabel("Rotations:");
    auto* rotationsSpinBox = new QSpinBox();
    rotationsSpinBox->setMinimum(0);
    rotationsSpinBox->setMaximum(3000);
    auto* directionCheckBox = new QCheckBox("clockwise");
    row2Layout->addWidget(downButton);
    row2Layout->addWidget(rotationsLabel);
    row2Layout->addWidget(rotationsSpinBox, 1);
    row2Layout->addWidget(directionCheckBox, 1);

    auto* numCirclesLabel = new QLabel("Circles:");
    auto* numCirclesSpinBox = new QSpinBox();
    numCirclesSpinBox->setMinimum(2);
    numCirclesSpinBox->setMaximum(10);
    auto* startStopButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaPlay), "Play");
    auto* helpButton = new QPushButton(style()->standardIcon(QStyle::SP_TitleBarContextHelpButton), "");
    helpButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    auto* row3Layout = new QHBoxLayout();
    row3Layout->addWidget(numCirclesLabel);
    row3Layout->addWidget(numCirclesSpinBox, 1);
    row3Layout->addWidget(startStopButton, 1);
    row3Layout->addWidget(helpButton);

    auto* layout = new QVBoxLayout();
    layout->addWidget(mView);
    layout->addLayout(row1Layout);
    layout->addLayout(row2Layout);
    layout->addLayout(row3Layout);
    setLayout(layout);
}

SpiralFun::Circle* AppWindow::addCircle(qreal radius)
{
    QPointF center(0.0, 0.0);
    if (!mCircles.empty())
    {
        const auto& prevCircle = *mCircles.back();
        center = prevCircle.getCenter();
        center.ry() -= prevCircle.getRadius() + radius;
    }

    auto circle = std::make_unique<SpiralFun::Circle>(mView, center, radius);
    mCircles.push_back(std::move(circle));
    return mCircles.back().get();
}

void AppWindow::advanceCircles(qreal angle)
{
    for(int i = 1; i < mCircles.size(); ++i)
        advanceCircle(i, angle);
}

void AppWindow::advanceCircle(unsigned index, qreal angle)
{
    if (index < 1 || index >= mCircles.size())
        throw std::out_of_range(std::string("index=") + std::to_string(index));

    const QPointF& rotationCenter = mCircles[index - 1]->getCenter();
    const int speed = mCircles[index]->getSpeed();
    const bool clockwise = speed > 0;

    for (unsigned i = index; i < mCircles.size(); ++i)
    {
        auto& circle = *mCircles[i];
        for (unsigned n = 0; n < std::abs(speed); ++n)
            circle.rotate(rotationCenter, angle, clockwise);
    }
}

}

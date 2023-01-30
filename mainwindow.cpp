#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showFullScreen();

    QGraphicsView* view = ui->graphicsView;
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScene.setItemIndexMethod(QGraphicsScene::NoIndex);
    mScene.setSceneRect(-500, -500, 1000, 1000);
    view->setScene(&mScene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

SpiralFun::Circle* MainWindow::addCircle(qreal radius)
{
    QPointF center(0.0, 0.0);
    if (!mCircles.empty())
    {
        const auto& prevCircle = *mCircles.back();
        center = prevCircle.getCenter();
        center.ry() -= prevCircle.getRadius() + radius;
    }

    auto circle = std::make_unique<SpiralFun::Circle>(&mScene, center, radius);
    mCircles.push_back(std::move(circle));
    return mCircles.back().get();
}

void MainWindow::advanceCircles(qreal angle)
{
    for(int i = 1; i < mCircles.size(); ++i)
        advanceCircle(i, angle);
}

void MainWindow::advanceCircle(unsigned index, qreal angle)
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

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();

    QGraphicsView* view = ui->graphicsView;
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    const QScreen* screen = view->screen();
    const QSize& screenSize = screen->size();
    qInfo() << "screen:" << screenSize << "dpr:" << screen->devicePixelRatio();

    mScene.setItemIndexMethod(QGraphicsScene::NoIndex);
    const qreal width = screenSize.width();
    const qreal height = screenSize.height();
    mScene.setSceneRect(-width / 2.0, -height / 2.0, width, height);
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

    auto circle = std::make_unique<SpiralFun::Circle>(ui->graphicsView, center, radius);
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

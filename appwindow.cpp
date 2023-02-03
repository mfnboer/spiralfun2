#include "appwindow.h"
#include <QBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QShowEvent>
#include <QStyle>

namespace SpiralFun {

namespace {
    constexpr int MIN_CIRCLES = 2;
    constexpr int MAX_CIRCLES = 10;
}

AppWindow::AppWindow()
{
    qInfo() << "screen:" << screen()->size() << "dpr:" << screen()->devicePixelRatio();

    mView = new GraphicsView();
    mView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    mScene = new QGraphicsScene();
    mScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    mScene->setBackgroundBrush(Qt::black);
    mView->setScene(mScene);
    mView->installEventFilter(this);

    auto* upButton = new QPushButton(style()->standardIcon(QStyle::SP_ArrowUp), "");
    upButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    QObject::connect(upButton, &QPushButton::clicked, this, &AppWindow::handleUp);
    auto* diameterLabel = new QLabel("Diameter:");
    mDiameterSpinBox = new SpinBox();
    mDiameterSpinBox->setMinimum(1);
    mDiameterSpinBox->setMaximum(300);
    QObject::connect(mDiameterSpinBox, &QSpinBox::valueChanged, this, &AppWindow::handleDiameter);
    mDrawCheckBox = new QCheckBox("draw");
    QObject::connect(mDrawCheckBox, &QCheckBox::clicked, this, &AppWindow::handleDraw);
    auto* row1Layout = new QHBoxLayout();
    row1Layout->addWidget(upButton);
    row1Layout->addWidget(diameterLabel);
    row1Layout->addWidget(mDiameterSpinBox, 1);
    row1Layout->addWidget(mDrawCheckBox, 1);

    auto* row2Layout = new QHBoxLayout();
    auto* downButton = new QPushButton(style()->standardIcon(QStyle::SP_ArrowDown), "");
    QObject::connect(downButton, &QPushButton::clicked, this, &AppWindow::handleDown);
    downButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    auto* rotationsLabel = new QLabel("Rotations:");
    mRotationsSpinBox = new SpinBox();
    mRotationsSpinBox->setMinimum(0);
    mRotationsSpinBox->setMaximum(3000);
    QObject::connect(mRotationsSpinBox, &QSpinBox::valueChanged, this, &AppWindow::handleRotations);
    mDirectionComboBox = new QComboBox();
    mDirectionComboBox->addItems({"clockwise", "counter clock"});
    QObject::connect(mDirectionComboBox, &QComboBox::currentIndexChanged, this,
                     [this](int index){ handleDirection(index == 0); });
    row2Layout->addWidget(downButton);
    row2Layout->addWidget(rotationsLabel);
    row2Layout->addWidget(mRotationsSpinBox, 1);
    row2Layout->addWidget(mDirectionComboBox, 1);

    auto* numCirclesLabel = new QLabel("Circles:");
    mNumCirclesComboBox = new QComboBox();
    for (int i = MIN_CIRCLES; i <= MAX_CIRCLES; ++i)
        mNumCirclesComboBox->addItem(QString::number(i));
    mNumCirclesComboBox->setWindowFlag(Qt::WindowStaysOnTopHint); // Workaround for combo box bug: no pop under
    QObject::connect(mNumCirclesComboBox, &QComboBox::currentIndexChanged, this,
                     [this](int index){ handleNumCircles(index + MIN_CIRCLES); });
    mStartStopButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaPlay), "Play");
    QObject::connect(mStartStopButton, &QPushButton::clicked, this, &AppWindow::handlePlay);
    auto* helpButton = new QPushButton(style()->standardIcon(QStyle::SP_TitleBarContextHelpButton), "");
    helpButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    auto* row3Layout = new QHBoxLayout();
    row3Layout->addWidget(numCirclesLabel);
    row3Layout->addWidget(mNumCirclesComboBox, 1);
    row3Layout->addWidget(mStartStopButton, 1);
    row3Layout->addWidget(helpButton);

    auto* layout = new QVBoxLayout();
    layout->addWidget(mView, 1);
    layout->addLayout(row1Layout);
    layout->addLayout(row2Layout);
    layout->addLayout(row3Layout);
    setLayout(layout);

    enableControls(false);
}

bool AppWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == mView && (event->type() == QEvent::Show || event->type() == QEvent::Resize))
    {
        qDebug() << "Show event, event:" << event->type() << event->spontaneous() <<
                    "circles:" << mCircles.size() << "view:" << mView->size();

        mView->fitInView(mScene->sceneRect(), Qt::KeepAspectRatioByExpanding);
    }

    return QWidget::eventFilter(obj, event);
}

void AppWindow::init()
{
    const qreal width = mView->width();
    const qreal height = mView->height();
    mScene->setSceneRect(-width / 2.0, -height / 2.0, width, height);
    mView->fitInView(mScene->sceneRect(), Qt::KeepAspectRatioByExpanding);
    const qreal minDimension = std::min(width, height);
    mDefaultCircleRadius = minDimension / 2.0 / (MAX_CIRCLES * 2 - 1);

    qInfo() << "init, scene rect:" << mScene->sceneRect() << "default radius:" << mDefaultCircleRadius;
}

void AppWindow::setupCircles()
{
    mCircles.clear();
    addCircle(mDefaultCircleRadius * 4);
    addCircle(mDefaultCircleRadius * 2)->setSpeed(1);
    addCircle(5)->setSpeed(-5)->setDraw(true);
    mCurrentIndex = 0;
    enableControls(true);
    mView->centerOn(mCircles[0]->GetEllipseItem());
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

void AppWindow::handleNumCircles(unsigned numCircles)
{
    if (numCircles == mCircles.size())
        return;

    if (numCircles < mCircles.size())
    {
        mCircles.resize(numCircles);
        if (mCurrentIndex >= mCircles.size())
        {
            mCurrentIndex = 0;
            setCurrentCircleFocus(true);
        }

        return;
    }

    Q_ASSERT(numCircles > mCircles.size());
    const int delta = numCircles - mCircles.size();
    for (int i = 0; i < delta; ++i)
        addCircle(mDefaultCircleRadius);
}

void AppWindow::handleDiameter(unsigned diameter)
{
    if (mCurrentIndex >= mCircles.size())
        return;

    auto& circle = mCircles[mCurrentIndex];
    const qreal radius = diameter / 2.0;
    const qreal diff = radius - circle->getRadius();
    QPointF center = circle->getCenter();
    center.ry() -= diff;
    circle->setRadius(radius);
    circle->setCenter(center);

    // Circle changed size, move circles on top of this circle.
    moveCircles(mCurrentIndex + 1, 2.0 * diff);
}

void AppWindow::moveCircles(unsigned index, qreal yShift)
{
    for (unsigned i = index; i < mCircles.size(); ++i)
    {
        auto& circle = mCircles[i];
        QPointF center = circle->getCenter();
        center.ry() -= yShift;
        circle->setCenter(center);
    }
}

void AppWindow::handleUp()
{
    if (mCircles.empty())
        return;

    if (mCurrentIndex < mCircles.size() - 1)
    {
        setCurrentCircleFocus(false);
        ++mCurrentIndex;
        setCurrentCircleFocus(true);
    }
}

void AppWindow::handleDown()
{
    if (mCurrentIndex > 0)
    {
        setCurrentCircleFocus(false);
        --mCurrentIndex;
        setCurrentCircleFocus(true);
    }
}

void AppWindow::handleDraw(bool draw)
{
    if (mCurrentIndex >= mCircles.size())
        return;

    mCircles[mCurrentIndex]->setDraw(draw);
}

void AppWindow::handleRotations(unsigned rotations)
{
    if (mCircles.empty())
        return;

    auto& circle = mCircles[mCurrentIndex];
    const int direction = circle->getSpeed() < 0 ? -1 : 1;
    circle->setSpeed(rotations * direction);
}

void AppWindow::handleDirection(bool clockwise)
{
    if (mCircles.empty())
        return;

    auto& circle = mCircles[mCurrentIndex];
    const int direction = clockwise ? 1 : -1;
    const int speed = std::abs(circle->getSpeed()) * direction;
    circle->setSpeed(speed);
}

void AppWindow::handlePlay()
{
    if (!mPlayer)
        mPlayer = std::make_unique<Player>(this);

    mPlayer->play();
    mStartStopButton->disconnect();
    mStartStopButton->setText("Stop");
    mStartStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    QObject::connect(mStartStopButton, &QPushButton::clicked, this, &AppWindow::handleStop);
    enableControls(false);
}

void AppWindow::handleStop()
{
    mPlayer = nullptr;
    mStartStopButton->disconnect();
    mStartStopButton->setText("Play");
    mStartStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    QObject::connect(mStartStopButton, &QPushButton::clicked, this, &AppWindow::handlePlay);
    resetScene();
    enableControls(true);
}

void AppWindow::setCurrentCircleFocus(bool focus)
{
    if (mCurrentIndex >= mCircles.size())
        return;

    auto& circle = mCircles[mCurrentIndex];
    circle->setFocus(focus);

    if (!focus)
        return;

    mDiameterSpinBox->setValue(std::round(circle->getRadius() * 2.0));
    mDrawCheckBox->setChecked(circle->getDraw());
    mRotationsSpinBox->setValue(std::abs(circle->getSpeed()));
    mDirectionComboBox->setCurrentIndex(circle->getSpeed() >= 0 ? 0 : 1);
    mNumCirclesComboBox->setCurrentIndex(mCircles.size() - MIN_CIRCLES);

    mRotationsSpinBox->setEnabled(mCurrentIndex > 1);
    mDirectionComboBox->setEnabled(mCurrentIndex > 0);
    mDrawCheckBox->setEnabled(mCurrentIndex > 0);
}

void AppWindow::enableControls(bool enable)
{
    mDiameterSpinBox->setEnabled(enable);
    mDrawCheckBox->setEnabled(enable);
    mRotationsSpinBox->setEnabled(enable);
    mDirectionComboBox->setEnabled(enable);
    mNumCirclesComboBox->setEnabled(enable);
    setCurrentCircleFocus(enable);

    if (enable)
    {
        // Workaround for bug in QT
        // The popup will sometimes pop under. Raise makes it visible again.
        mNumCirclesComboBox->raise();
    }
}

void AppWindow::resetCircles()
{
    if (mCircles.empty())
        return;

    QPointF center(0.0, 0.0);
    mCircles[0]->setCenter(center);

    for (int i = 1; i < mCircles.size(); ++i)
    {
        auto& circle = mCircles[i];
        auto& prev = mCircles[i - 1];
        center.ry() -= prev->getRadius() + circle->getRadius();
        circle->setCenter(center);
    }
}

void AppWindow::resetScene()
{
    for (auto& circle : mCircles)
        circle->removeFromScene();

    mScene->clear();
    resetCircles();

    for (auto& circle : mCircles)
        circle->addToScene();
}

}

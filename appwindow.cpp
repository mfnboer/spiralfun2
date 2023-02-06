#include "appwindow.h"
#include "image_button.h"
#include <QBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QScreen>
#include <QShowEvent>
#include <QStyle>

namespace SpiralFun {

namespace {
constexpr char const* APP_NAME = "Spiral Fun";
constexpr int MIN_CIRCLES = 2;
constexpr int MAX_CIRCLES = 10;
constexpr int MAX_DIAMETER = 300;
constexpr int MAX_ROTATIONS = 9999;

const std::initializer_list<CircleConfig> EXAMPLE1_CONFIG = {
    { 6.0, 0, false, Qt::blue },
    { 2.6, 1, false, Qt::green },
    { 1.6, 5, false, Qt::yellow },
    { 0.8, 40, true, Qt::red }
};
const std::initializer_list<CircleConfig> EXAMPLE2_CONFIG = {
    { 5.0, 0, false, Qt::blue },
    { 3.0, 1, false, Qt::yellow },
    { 1.6, 4, true, Qt::red },
    { 0.2, 200, true, Qt::white }
};
const std::initializer_list<CircleConfig> EXAMPLE3_CONFIG = {
    { 6.0, 0, false, Qt::blue },
    { 2.1, 1, false, Qt::green },
    { 1.6, -5, false, Qt::yellow },
    { 0.8, 25, true, Qt::red },
    { 0.4, -125, false, Qt::cyan },
    { 0.08, 625, true, Qt::white }
};
const std::initializer_list<CircleConfig> EXAMPLE4_CONFIG = {
    { 4.8, 0, false, Qt::cyan },
    { 2.8, 1, false, Qt::yellow },
    { 1.6, -3, true, Qt::green },
    { 0.8, 9, false, Qt::magenta },
    { 0.4, -27, true, Qt::blue },
    { 0.2, 81, false, Qt::red },
    { 0.04, -243, true, Qt::white }
};
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
    mDiameterSpinBox->setMaximum(MAX_DIAMETER);
    QObject::connect(mDiameterSpinBox, &QSpinBox::valueChanged, this, &AppWindow::handleDiameter);
    mDrawColorButton = new ColorSelectButton();
    QObject::connect(mDrawColorButton, &ColorSelectButton::colorChanged, this, &AppWindow::handleColor);
    mDrawCheckBox = new QCheckBox("draw");
    QObject::connect(mDrawCheckBox, &QCheckBox::clicked, this, &AppWindow::handleDraw);
    auto* row1Layout = new QHBoxLayout();
    row1Layout->addWidget(upButton);
    row1Layout->addWidget(diameterLabel);
    row1Layout->addWidget(mDiameterSpinBox, 1);
    row1Layout->addWidget(mDrawColorButton, 1);
    row1Layout->addWidget(mDrawCheckBox, 1);

    auto* row2Layout = new QHBoxLayout();
    auto* downButton = new QPushButton(style()->standardIcon(QStyle::SP_ArrowDown), "");
    QObject::connect(downButton, &QPushButton::clicked, this, &AppWindow::handleDown);
    downButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    auto* rotationsLabel = new QLabel("Rotations:");
    mRotationsSpinBox = new SpinBox();
    mRotationsSpinBox->setMinimum(0);
    mRotationsSpinBox->setMaximum(MAX_ROTATIONS);
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
    mNumCirclesSpinBox = new SpinBox();
    mNumCirclesSpinBox->setMinimum(MIN_CIRCLES);
    mNumCirclesSpinBox->setMaximum(MAX_CIRCLES);
    QObject::connect(mNumCirclesSpinBox, &SpinBox::valueChanged, this, &AppWindow::handleNumCircles);
    mStartStopButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaPlay), "Play");
    QObject::connect(mStartStopButton, &QPushButton::clicked, this, &AppWindow::handlePlay);
    mMoreButton = new QPushButton("\u2630");
    mMoreButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    QObject::connect(mMoreButton, &QPushButton::clicked, this, [this](){
        // Create new window on every button push as work around for Android.
        // After showing a menu once, it will not show again on Android.
        QMenu moreMenu;
        moreMenu.addAction("Examples", this, &AppWindow::examples);
        moreMenu.addAction("Help", this, &AppWindow::helpInfo);
        moreMenu.addAction("About", this, &AppWindow::aboutInfo);
        // Set a minimum width, otherwise the menu is too narrow on Android.
        moreMenu.setMinimumWidth(150);
        moreMenu.exec(QCursor::pos());
    });

    auto* row3Layout = new QHBoxLayout();
    row3Layout->addWidget(numCirclesLabel);
    row3Layout->addWidget(mNumCirclesSpinBox, 1);
    row3Layout->addWidget(mStartStopButton, 1);
    row3Layout->addWidget(mMoreButton);

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

void AppWindow::setupCircles(const std::vector<CircleConfig>& config)
{
    mCircles.clear();
    mScene->clear();

    for (const auto& c : config)
    {
        addCircle(mDefaultCircleRadius * c.mRelRadius)->
                setSpeed(c.mSpeed)->
                setDraw(c.mDraw)->
                setColor(c.mColor);
    }

    mCurrentIndex = 0;
    enableControls(true);
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

void AppWindow::forceDraw()
{
    for (auto& circle : mCircles)
    {
        if (circle->getDraw())
            circle->forceDrawToCenter();
    }
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
        addCircle(mDefaultCircleRadius)->setSpeed(1);
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

void AppWindow::handleColor(const QColor& color)
{
    if (mCurrentIndex >= mCircles.size())
        return;

    mCircles[mCurrentIndex]->setColor(color);
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
    mDrawColorButton->setColor(circle->getColor());
    mDrawCheckBox->setChecked(circle->getDraw());
    mRotationsSpinBox->setValue(std::abs(circle->getSpeed()));
    mDirectionComboBox->setCurrentIndex(circle->getSpeed() >= 0 ? 0 : 1);
    mNumCirclesSpinBox->setValue(mCircles.size());

    mRotationsSpinBox->setEnabled(mCurrentIndex > 1);
    mDirectionComboBox->setEnabled(mCurrentIndex > 0);
    mDrawCheckBox->setEnabled(mCurrentIndex > 0);
}

void AppWindow::enableControls(bool enable)
{
    mDiameterSpinBox->setEnabled(enable);
    mDrawColorButton->setEnabled(enable);
    mDrawCheckBox->setEnabled(enable);
    mRotationsSpinBox->setEnabled(enable);
    mDirectionComboBox->setEnabled(enable);
    mNumCirclesSpinBox->setEnabled(enable);
    mMoreButton->setEnabled(enable);
    setCurrentCircleFocus(enable);

    mMoreButton->raise();
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

void AppWindow::addCirclesToScene()
{
    for (auto& circle : mCircles)
        circle->addToScene();
}

void AppWindow::removeCirclesFromScene()
{
    for (auto& circle : mCircles)
        circle->removeFromScene();
}

void AppWindow::resetScene()
{
    removeCirclesFromScene();
    mScene->clear();
    resetCircles();
    addCirclesToScene();
}

void AppWindow::examples()
{   
    auto* w = new QDialog(mView);
    auto* layout = new QGridLayout;

    int row = 0;
    int column = 0;
    auto addExample = [this, w, layout, &row, &column]
            (const QString& fileName, const std::vector<CircleConfig>& config){
            auto* button = new ImageButton(fileName);
            QObject::connect(button, &QPushButton::clicked, w, [this, w, config]{
                    setupCircles(config);
                    w->accept();
            });
            layout->addWidget(button, row, column++);

            if (column > 1)
            {
                ++row;
                column = 0;
            }
    };

    addExample(":/images/example1.png", EXAMPLE1_CONFIG);
    addExample(":/images/example2.png", EXAMPLE2_CONFIG);
    addExample(":/images/example3.png", EXAMPLE3_CONFIG);
    addExample(":/images/example4.png", EXAMPLE4_CONFIG);

    QPushButton* pb = new QPushButton("Cancel");
    QObject::connect(pb, &QPushButton::clicked, w, &QDialog::reject);
    layout->addWidget(pb, 2, 1);
    w->setLayout(layout);
    w->move(50, 50);

    QObject::connect(w, &QDialog::finished, w, &QDialog::deleteLater);
    w->open();
}

void AppWindow::helpInfo()
{
    QMessageBox::information(this, APP_NAME,
        "Help 1\n"
        "Help 2");
}

void AppWindow::aboutInfo()
{
    QMessageBox::information(this, APP_NAME,
        "<center>"
        "<b>Spiral Fun</b>"
        "<p>"
        "Created by Michel de Boer<br>"
        "\u00A9 2023"
        "</center>");
}

// TODO
void AppWindow::saveImage(const QString& fileName) const
{
    QPixmap pixmap = mView->grab(QRect(469, 25, 880, 880));
    pixmap.save(fileName);
}

}

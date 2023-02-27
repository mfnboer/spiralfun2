// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "spiral_scene.h"
#include "circle.h"
#include "utils.h"
#include "player.h"
#include <QFile>
#include <QQmlEngine>
#include <QQuickItemGrabResult>
#include <QQuickWindow>
#include <QSGFlatColorMaterial>
#include <QSGNode>

namespace SpiralFun {

namespace {

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

SpiralScene::SpiralScene(QQuickItem *parent) :
    QQuickItem(parent)
{
    setFlags(ItemHasContents | ItemIsViewport);
    setAntialiasing(true);
    setClip(true);
    setAcceptTouchEvents(true);

    // The window size is not yet known at this time. But setting up a scene
    // guarantees there are always circles available.
    setupCircles();
}

void SpiralScene::init()
{
    const qreal minDimension = std::min(width(), height());
    mDefaultCircleRadius = minDimension / 2.0 / (MAX_CIRCLES * 2 - 1);
    qDebug() << "Scene size:" << size() << "default-radius:" << mDefaultCircleRadius;
    qDebug() << "Screen:" << window()->screen()->size() << "dpr:" << window()->devicePixelRatio() << window()->screen()->devicePixelRatio();
}

void SpiralScene::setupCircles(const std::vector<CircleConfig>& config)
{
    mCircles.clear();

    for (const auto& c : config)
    {
        addCircle(mDefaultCircleRadius * c.mRelRadius)->
                setSpeed(c.mSpeed)->
                setDraw(c.mDraw)->
                setColor(c.mColor);
    }

    setCurrentIndex(0);
    setCurrentCircleFocus(true);
    emit numCirclesChanged();
}

void SpiralScene::setupExample(const QString& example)
{
    const std::unordered_map<QString, std::initializer_list<CircleConfig>> configs = {
        { "/images/example1.png", EXAMPLE1_CONFIG },
        { "/images/example2.png", EXAMPLE2_CONFIG },
        { "/images/example3.png", EXAMPLE3_CONFIG },
        { "/images/example4.png", EXAMPLE4_CONFIG }
    };

    auto it = configs.find(example);
    if (it == configs.end())
    {
        qWarning() << "Example not found:" << example;
        return;
    }

    setupCircles(it->second);
}

Circle* SpiralScene::getCurrentCircle() const
{
    Q_ASSERT(!mCircles.empty());
    return mCurrentIndex < mCircles.size() ? mCircles[mCurrentIndex].get() : nullptr;
}

void SpiralScene::setNumCircles(int numCircles)
{
    qDebug() << "numCircles:" << numCircles << "current:" << mCircles.size();
    const unsigned n = static_cast<unsigned>(numCircles);

    if (n == mCircles.size())
        return;

    if (n < mCircles.size())
    {
        mCircles.resize(numCircles);
        if (mCurrentIndex >= mCircles.size())
        {
            setCurrentIndex(mCircles.size() - 1);
            setCurrentCircleFocus(true);
        }

        emit numCirclesChanged();
        return;
    }

    Q_ASSERT(n > mCircles.size());
    const int delta = n - mCircles.size();
    for (int i = 0; i < delta; ++i)
        addCircle(mDefaultCircleRadius)->setSpeed(1)->setDraw(true);

    setCurrentCircleFocus(false);
    setCurrentIndex(mCircles.size() - 1);
    setCurrentCircleFocus(true);
    emit numCirclesChanged();
}

SpiralFun::Circle* SpiralScene::addCircle(qreal radius)
{
    QPointF center = boundingRect().center();
    if (!mCircles.empty())
    {
        const auto& prevCircle = *mCircles.back();
        center = prevCircle.getCenter();
        center.ry() -= prevCircle.getRadius() + radius;
    }

    auto circle = std::make_unique<SpiralFun::Circle>(this);
    auto* engine = qmlEngine(this);
    engine->setContextForObject(circle.get(), qmlContext(this));
    circle->setCenter(center);
    circle->setRadius(radius);
    QObject::connect(circle.get(), &Circle::diameterChanged, this,
            [this, c=circle.get()](int oldDiameter){ handleDiameterChange(c, oldDiameter); });
    mCircles.push_back(std::move(circle));
    return mCircles.back().get();
}

std::optional<unsigned> SpiralScene::findCircle(const Circle* circle)
{
    const auto it = std::find_if(mCircles.begin(), mCircles.end(), [circle](const auto& c){
        return c.get() == circle;
    });

    if (it == mCircles.end())
        return {};

    return it - mCircles.begin();
}

void SpiralScene::handleDiameterChange(Circle* circle, int oldDiameter)
{
    std::optional<unsigned> index = findCircle(circle);

    if (!index)
    {
        qWarning() << "Circle not found:" << circle;
        return;
    }

    const int diff = circle->getDiameter() - oldDiameter;
    qreal yShift = diff;

    if (*index > 0)
    {
        QPointF center = circle->getCenter();
        center.ry() -= diff / 2.0;
        circle->setCenter(center);
    }
    else
    {
        yShift = diff / 2.0;
    }

    // Circle changed size, move circles on top of this circle.
    moveCircles(mCurrentIndex + 1, yShift);
}

void SpiralScene::moveCircles(unsigned index, qreal yShift)
{
    for (unsigned i = index; i < mCircles.size(); ++i)
    {
        auto& circle = mCircles[i];
        QPointF center = circle->getCenter();
        center.ry() -= yShift;
        circle->setCenter(center);
    }
}

void SpiralScene::circleUp()
{
    if (mCircles.empty())
        return;

    if (mCurrentIndex < mCircles.size() - 1)
    {
        setCurrentCircleFocus(false);
        setCurrentIndex(mCurrentIndex + 1);
        setCurrentCircleFocus(true);
    }
}

void SpiralScene::circleDown()
{
    if (mCurrentIndex > 0)
    {
        setCurrentCircleFocus(false);
        setCurrentIndex(mCurrentIndex - 1);
        setCurrentCircleFocus(true);
    }
}

void SpiralScene::setCurrentCircleFocus(bool focus)
{
    if (mCurrentIndex >= mCircles.size())
        return;

    auto& circle = mCircles[mCurrentIndex];
    circle->setFocus(focus);

    if (!focus)
        return;

    emit currentCircleChanged();
}

void SpiralScene::setCurrentIndex(unsigned index)
{
    if (mCurrentIndex == index)
        return;

    mCurrentIndex = index;
    emit currentCircleIndexChanged();
}

void SpiralScene::play()
{
    mLineSegmentCount = 0;
    setCurrentCircleFocus(false);
    mPlayer = std::make_unique<Player>(mCircles);
    QObject::connect(mPlayer.get(), &Player::refreshScene, this, [this]{ update(); });
    QObject::connect(mPlayer.get(), &Player::done, this, [this]{
            removeCirclesFromScene();
            setPlayState(DONE_PLAYING);
            qDebug() << "Line segments drawn:" << calcTotalLineSements();
    });
    mPlayer->play();
    setPlayState(PLAYING);
}

void SpiralScene::stop()
{
    mPlayer = nullptr;
    resetScene();
    setPlayState(NOT_PLAYING);
    setCurrentCircleFocus(true);
}

void SpiralScene::setPlayState(PlayState state)
{
    mPlayState = state;
    emit playStateChanged();
}

uint64_t SpiralScene::calcTotalLineSements() const
{
    uint64_t total = mLineSegmentCount;

    // Not all line segements may be rendered yet. Add what is buffered.
    for (const auto& [_, line] : mLines)
    {
        if (line.mLinePoints.size() > 0)
            total += line.mLinePoints.size() - 1;
    }

    return total;
}

void SpiralScene::addCirclesToScene()
{
    for (auto& circle : mCircles)
        circle->addToScene();
}

void SpiralScene::removeCirclesFromScene()
{
    for (auto& circle : mCircles)
        circle->removeFromScene();
}

void SpiralScene::resetCircles()
{
    if (mCircles.empty())
        return;

    QPointF center = mCircles[0]->getCenter();
    for (unsigned i = 1; i < mCircles.size(); ++i)
    {
        auto& circle = mCircles[i];
        auto& prev = mCircles[i - 1];
        center.ry() -= prev->getRadius() + circle->getRadius();
        circle->setCenter(center);
    }
}

void SpiralScene::resetScene()
{
    removeCirclesFromScene();
    mClearScene = true;
    resetCircles();
    addCirclesToScene();
    mScaleFactor = 1.0;
    setScale(mScaleFactor);
    update();
    mSavedImageFileName.clear();
}

void SpiralScene::selectCircle(Circle* circle)
{
    const auto it = std::find_if(mCircles.begin(), mCircles.end(), [circle](const auto& c){
        return c.get() == circle;
    });

    if (it == mCircles.end())
        return;

    const unsigned index = it - mCircles.begin();
    if (index != mCurrentIndex)
    {
        setCurrentCircleFocus(false);
        setCurrentIndex(index);
        setCurrentCircleFocus(true);
    }
}

Circle::Line* SpiralScene::addLine(QObject* object, const QColor& color, const QPointF& startPoint)
{
    Circle::Line& line = mLines[object];
    line.mColor = color;
    line.mLinePoints.reserve(256);
    line.mLinePoints.push_back(startPoint);
    return &line;
}

void SpiralScene::removeLine(QObject* object)
{
    mLines.erase(object);
}

QSGNode* SpiralScene::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    QSGNode* sceneRoot = (oldNode ? oldNode : new QSGNode);

    if (mClearScene)
    {
        sceneRoot->removeAllChildNodes();
        mClearScene = false;
    }

    for (auto& [_, line] : mLines)
    {
        if (line.mLinePoints.size() < 2)
            continue;

        if (!line.mRoot)
        {
            line.mRoot = new QSGNode;
            sceneRoot->appendChildNode(line.mRoot);
        }

        auto* node = createLineNode(line);
        line.mRoot->appendChildNode(node);
        QPointF p = line.mLinePoints.back();
        line.mLinePoints.clear();
        line.mLinePoints.push_back(p);
    }

    return sceneRoot;
}

QSGNode* SpiralScene::createLineNode(const Circle::Line& line)
{
    auto* node = new QSGGeometryNode;
    auto* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), line.mLinePoints.size());
    geometry->setLineWidth(1);
    geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsGeometry);
    auto* material = new QSGFlatColorMaterial;
    material->setColor(line.mColor);
    node->setMaterial(material);
    node->setFlag(QSGNode::OwnsMaterial);

    auto* vertices = geometry->vertexDataAsPoint2D();
    for (int unsigned i = 0; i < line.mLinePoints.size(); ++i)
    {
        const QPointF& p = line.mLinePoints[i];
        vertices[i].set(p.x(), p.y());
    }

    node->markDirty(QSGNode::DirtyGeometry);
    mLineSegmentCount += line.mLinePoints.size() - 1;
    return node;
}

void SpiralScene::touchEvent(QTouchEvent* event)
{
    switch (event->type())
    {
    case QEvent::TouchCancel:
        event->accept();
        return;
    case QEvent::TouchBegin:
    case QEvent::TouchEnd:
    case QEvent::TouchUpdate:
        break;
    default:
        qWarning() << "Unknown touch event:" << event->type();
        return;
    }

    QTouchEvent* touch = static_cast<QTouchEvent*>(event);
    if (touch->points().size() == 2)
    {
        const QEventPoint& p1 = touch->points()[0];
        const QEventPoint& p2 = touch->points()[1];
        const qreal currentDist = QLineF(p1.position(), p2.position()).length();
        const qreal startDist = QLineF(p1.pressPosition(), p2.pressPosition()).length();
        qreal currentScaleFactor = currentDist / startDist;

        if (touch->touchPointStates() & QEventPoint::Released)
        {
            // If one finger is temporarily released, remember the current cale factor.
            mScaleFactor *= currentScaleFactor;
            currentScaleFactor = 1.0;
        }

        setScale(mScaleFactor * currentScaleFactor);
    }

    event->accept();
}

void SpiralScene::saveImage(bool share)
{
    const qreal dpr = window()->effectiveDevicePixelRatio();
    const QSize imageSize = (size() * dpr).toSize();
    auto grabResult = grabToImage(imageSize);
    if (!grabResult)
    {
        emit message("Failed to grab image.");
        return;
    }

    const QString picPath = Utils::getPicturesPath();
    if (picPath.isNull())
    {
        emit message("Cannot save file.");
        return;
    }

    const QString fileName = picPath + "/" + Utils::createPictureFileName();
    QObject::connect(grabResult.get(), &QQuickItemGrabResult::ready, this,
        [this, grabResult, fileName, share]{
            if (grabResult->saveToFile(fileName))
            {
                qDebug() << "Saved file:" << fileName;
                emit message(QString("Saved file: %1").arg(fileName));
                Utils::scanMediaFile(fileName, share);
                mSavedImageFileName = fileName;
            }
            else
            {
                emit message(QString("Failed to save: %1").arg(fileName));
            }
        });

    return;
}

void SpiralScene::shareImage()
{
    if (!mSavedImageFileName.isNull() && QFile::exists(mSavedImageFileName))
    {
        qDebug() << "File already saved, share saved file:" << mSavedImageFileName;
        Utils::scanMediaFile(mSavedImageFileName, true);
    }
    else
    {
        saveImage(true);
    }
}

}

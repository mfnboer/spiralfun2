// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "spiral_scene.h"
#include "circle.h"
#include "exception.h"
#include "jni_callback.h"
#include "player.h"
#include "utils.h"
#include <QFile>
#include <QQmlEngine>
#include <QQuickItemGrabResult>
#include <QSGFlatColorMaterial>
#include <QSGNode>

using namespace std::chrono_literals;

namespace SpiralFun {

SpiralScene::SpiralScene(QQuickItem *parent) :
    QQuickItem(parent)
{
    setFlags(ItemHasContents | ItemIsViewport);
    setAntialiasing(true);
    setAcceptTouchEvents(true);

    // The window size is not yet known at this time. But setting up a scene
    // guarantees there are always circles available.
    setupCircles();
}

void SpiralScene::init()
{
    calcDefaultRadiusSize();

    // Now the window size is known, setup with proper position and sizes
    setupCircles();
    QObject::connect(this, &QQuickItem::widthChanged, this, [this]{ handleWindowSizeChanged(); });
    QObject::connect(this, &QQuickItem::heightChanged, this, [this]{ handleWindowSizeChanged(); });

    auto& jniCallbackListener = JNICallbackListener::getInstance();
    QObject::connect(&jniCallbackListener, &JNICallbackListener::viewUriReceived,
                     this, [this](const QString& uri){ handleReceivedAndroidIntent(uri); });
    QObject::connect(&jniCallbackListener, &JNICallbackListener::mediaScannerFinished,
                     this, [this](const QString& uri){ handleMediaScannerFinished(uri); });

    // Handle possibly pending intent from Android.
    Utils::handlePendingIntent();

    qInfo() << "Surface format:" << window()->format();
}

void SpiralScene::calcDefaultRadiusSize()
{
    const qreal minDimension = std::min(width(), height());
    mDefaultCircleRadius = minDimension / 2.0 / (MAX_CIRCLES * 2 - 1);
    const int oldMaxDiameter = MAX_DIAMETER;
    MAX_DIAMETER = SpiralConfig::MAX_REL_RADIUS * mDefaultCircleRadius * 2;

    if (oldMaxDiameter != MAX_DIAMETER)
        emit maxDiameterChanged();

    qDebug() << "Scene size:" << size() << "default-radius:" << mDefaultCircleRadius << "max-diameter:" << MAX_DIAMETER;
    qDebug() << "Screen:" << window()->screen()->size() << "dpr:" << window()->devicePixelRatio() << window()->screen()->devicePixelRatio();
}

void SpiralScene::handleWindowSizeChanged()
{
    calcDefaultRadiusSize();
    stop();
}

void SpiralScene::setupCircles(const CircleConfigList& config)
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
    moveCircles(*index + 1, yShift);
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

bool SpiralScene::checkPlayRequirement()
{
    const auto it = std::find_if(mCircles.begin(), mCircles.end(), [](const auto& c){
            return c->getDraw(); });

    if (it == mCircles.end())
    {
        emit message("Enable line drawing on at least 1 circle");
        return false;
    }

    return true;
}

void SpiralScene::play()
{
    if (!checkPlayRequirement())
        return;

    setPlayState(PLAYING);
    doPlay(nullptr);
}

void SpiralScene::playSequence(const QVariant& mutations, int sequenceLength, MutationSequence::SaveAs saveAs)
{
    Q_ASSERT(sequenceLength > 0);
    if (!checkPlayRequirement())
        return;

    mMutationSequence = std::make_unique<MutationSequence>(mCircles, *this);
    mMutationSequence->setSequenceLength(sequenceLength);
    mMutationSequence->setMutations(mutations);
    emit sequenceLengthChanged();

    removeCirclesFromScene();
    setPlayState(PLAYING_SEQUENCE);

    connect(mMutationSequence.get(), &MutationSequence::sequenceFramePlaying, this, [this]{ emit sequenceFrameChanged(); });
    connect(mMutationSequence.get(), &MutationSequence::sequenceFinished, this, [this]{
            setPlayState(DONE_PLAYING);
            mMutationSequence = nullptr;
        });

    mMutationSequence->play(saveAs);
}

void SpiralScene::playSequence()
{
    doPlay(nullptr);
}

void SpiralScene::doPlay(std::unique_ptr<SceneGrabber> recorder)
{
    mStats = {};
    setCurrentCircleFocus(false);
    mPlayer = std::make_unique<Player>(mCircles);
    QObject::connect(mPlayer.get(), &Player::refreshScene, this, [this]{ update(); });
    QObject::connect(mPlayer.get(), &Player::angleChanged, this, [this]{ emit playAngleChanged(); });
    QObject::connect(mPlayer.get(), &Player::done, this, [this](const Player::Stats& stats){
            mStats.mPlayerStats = stats;
            removeCirclesFromScene();

            if (mPlayState == RECORDING)
            {
                const QString fileName = mPlayer->getGifFileName().split('/').last();
                emit statusUpdate(QString("GIF saved: %1").arg(fileName));
            }

            if (mPlayState == PLAYING_SEQUENCE)
            {
                mDoRender = true;
                mClearScene = true;
            }
            else
            {
                setPlayState(DONE_PLAYING);
            }

            update();
    });

    switch (mPlayState)
    {
    case PLAYING:
    case RECORDING:
        mPlayer->play(std::move(recorder));
        break;
    case PLAYING_SEQUENCE:
        mPlayer->playAll();
        break;
    default:
        Q_ASSERT(false);
    }
}

void SpiralScene::showSpiralStats()
{
    const QString statMsg = QString(
        "| Statistic      | Value |\n"
        "| :------------- | ----: |\n"
        "| Creation steps | %1    |\n"
        "| Creation time  | %2 s  |\n"
        "| Step time      | %3 ms |\n"
        "| Line segments  | %4    |")
            .arg(mStats.mPlayerStats.mCycles)
            .arg(qreal(mStats.mPlayerStats.mPlayTime / 1000.0ms))
            .arg(qreal(mStats.mPlayerStats.mPlayTime / (qreal)mStats.mPlayerStats.mCycles / 1000.0us))
            .arg(mStats.mLineSegmentCount);

    emit message(statMsg);
}

void SpiralScene::record()
{
    auto recorder = std::make_unique<SceneGrabber>(this, mSceneRect);
    resetScene();
    setPlayState(RECORDING);
    setShareMode(SHARE_VID);
    mShareContentUri.clear();
    doPlay(std::move(recorder));
}

void SpiralScene::stop()
{
    mPlayer = nullptr;
    mMutationSequence = nullptr;
    resetScene();
    setPlayState(NOT_PLAYING);
    setCurrentCircleFocus(true);
    setShareMode(SHARE_PIC);
}

void SpiralScene::setPlayState(PlayState state)
{
    switch (state)
    {
    case NOT_PLAYING:
    case PLAYING:
    case RECORDING:
    case DONE_PLAYING:
        mDoRender = true;
        break;
    case PLAYING_SEQUENCE:
        mDoRender = false;
        break;
    }

    mPlayState = state;  
    emit playStateChanged();
}

void SpiralScene::setShareMode(ShareMode shareMode)
{
    mShareMode = shareMode;
    emit shareModeChanged();
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

    QPointF center = boundingRect().center();
    mCircles[0]->setCenter(center);

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
    for (auto& [_, line] : mLines)
        line.mLinePoints.clear();

    removeCirclesFromScene();
    mClearScene = true;
    resetCircles();
    addCirclesToScene();
    mScaleFactor = 1.0;
    setScale(mScaleFactor);
    update();
    setSharingInProgress(false);
    mShareContentUri.clear();
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

ScopedLine SpiralScene::addLine(QObject* object, const QColor& color, int lineWidth, const QPointF& startPoint)
{
    Q_ASSERT(!mLines.count(object));
    Line& line = mLines[object];
    line.mColor = color;
    line.mLineWidth = lineWidth;
    line.mLinePoints.reserve(256);
    line.mLinePoints.push_back(startPoint);
    return std::forward<ScopedLine>(ScopedLine(&line, [this, object]{ mLines.erase(object); }));
}

QSGNode* SpiralScene::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    if (!mDoRender)
        return oldNode;

    QSGNode* sceneRoot = (oldNode ? oldNode : new QSGNode);

    if (mClearScene)
    {
        for (auto& [_, line] : mLines)
            line.mRoot = nullptr;

        delete sceneRoot;
        sceneRoot = new QSGNode;
        mSceneRect = {};
        mClearScene = false;
    }

    for (auto& [_, line] : mLines)
    {
        if (line.mLinePoints.size() < 2)
            continue;

        if (!line.mRoot)
        {
            line.mRoot = new QSGNode;
            line.mRoot->setFlag(QSGNode::OwnedByParent);
            sceneRoot->appendChildNode(line.mRoot);
        }

        auto* node = createLineNode(line);
        node->setFlag(QSGNode::OwnedByParent);
        line.mRoot->appendChildNode(node);
        QPointF p = line.mLinePoints.back();
        line.mLinePoints.clear();
        line.mLinePoints.push_back(p);
    }

    if (mPlayState == PLAYING_SEQUENCE)
    {
        mDoRender = false;
        emit sequenceFramePlayed();
    }

    return sceneRoot;
}

QSGNode* SpiralScene::createLineNode(const Line& line)
{
    auto* node = new QSGGeometryNode;
    auto* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), line.mLinePoints.size());
    geometry->setLineWidth(line.mLineWidth);
    geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsGeometry);
    auto* material = new QSGFlatColorMaterial;
    material->setColor(line.mColor);
    material->setFlag(QSGMaterial::Blending);
    node->setMaterial(material);
    node->setFlag(QSGNode::OwnsMaterial);

    auto* vertices = geometry->vertexDataAsPoint2D();
    for (int unsigned i = 0; i < line.mLinePoints.size(); ++i)
    {
        const QPointF& p = line.mLinePoints[i];
        vertices[i].set(p.x(), p.y());
        updateSceneRect(p);
    }

    node->markDirty(QSGNode::DirtyGeometry);
    mStats.mLineSegmentCount += line.mLinePoints.size() - 1;
    mStats.mLinePointsSum += line.mLinePoints.size();
    ++mStats.mLineCount;
    return node;
}

void SpiralScene::updateSceneRect(const QPointF& p)
{
    const qreal x = std::clamp(p.x(), 0.0, size().width());
    const qreal y = std::clamp(p.y(), 0.0, size().height());

    if (mSceneRect.isNull())
    {
        mSceneRect = QRectF(QPointF(x, y), QSizeF(1, 1));
        return;
    }

    if (x < mSceneRect.left())
        mSceneRect.setLeft(x);
    else if (x > mSceneRect.right())
        mSceneRect.setRight(x);

    if (y < mSceneRect.top())
        mSceneRect.setTop(y);
    else if (y > mSceneRect.bottom())
        mSceneRect.setBottom(y);
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
            // If one finger is temporarily released, remember the current scale factor.
            mScaleFactor *= currentScaleFactor;
            currentScaleFactor = 1.0;
        }

        setScale(mScaleFactor * currentScaleFactor);
    }

    event->accept();
}

bool SpiralScene::saveImage(const QRectF cutRect, const QString& baseNameSuffix,
                            const SavedCallback& savedCallback)
{
    QString picPath;
    try {
        picPath = Utils::getPicturesPath();
    } catch (RuntimeException& e) {
        emit message(e.msg());
        return false;
    }

    if (picPath.isNull())
    {
        emit message("Cannot save file.");
        return false;
    }

    const QString baseFileName = Utils::createPictureFileName(baseNameSuffix);
    const QString fileName = picPath + "/" + baseFileName;
    if (QFile::exists(fileName))
    {
        emit message(QString("Failed to create: %1").arg(fileName));
        return false;
    }

    const QRectF grabRect = cutRect.isNull() ? mSceneRect : cutRect;
    mSceneGrabber = std::make_unique<SceneGrabber>(this, grabRect);
    const bool grabbed = mSceneGrabber->grabScene(
        [this, fileName, baseFileName, savedCallback](const QImage& img){
            if (img.save(fileName))
            {
                qDebug() << "Saved file:" << fileName;
                emit statusUpdate(QString("Image saved: %1").arg(baseFileName));
                Utils::scanMediaFile(fileName);

                if (savedCallback)
                    savedCallback(true);
            }
            else
            {
                emit message(QString("Failed to save: %1").arg(fileName));
                setSharingInProgress(false);

                if (savedCallback)
                    savedCallback(false);
            }
        });

    if (!grabbed)
        emit message("Failed to grab image.");

    return grabbed;
}

void SpiralScene::share()
{
    switch (mShareMode)
    {
    case SHARE_PIC:
        shareImage();
        break;
    case SHARE_VID:
        shareVideo();
        break;
    }
}

void SpiralScene::shareImage()
{
    setSharingInProgress(true);

    if (!mShareContentUri.isNull())
    {
        qDebug() << "Share content uri available:" << mShareContentUri;
        shareContent();
    }
    else
    {
        if (!saveImage())
            setSharingInProgress(false);
    }
}

void SpiralScene::shareContent()
{
    Q_ASSERT(!mShareContentUri.isNull());
    SpiralConfig cfg(mCircles, mDefaultCircleRadius);
    const QString configAppUri = cfg.getConfigAppUri();

    QString mimeType;
    switch (mShareMode)
    {
    case SHARE_PIC:
        mimeType = "image/jpg";
        break;
    case SHARE_VID:
        mimeType = "image/gif";
        break;
    }

    Utils::sharePicture(mShareContentUri, configAppUri, mimeType);
}

void SpiralScene::shareVideo()
{
    if (mShareMode != SHARE_VID || mShareContentUri.isNull())
    {
        qDebug() << "No video to share";
        return;
    }

    setSharingInProgress(true);
    shareContent();
    setSharingInProgress(false);
}

void SpiralScene::handleMediaScannerFinished(const QString& contentUri)
{
    if (mShareContentUri.isNull() && (mShareMode == SHARE_VID || mSharingInProgress))
    {
        mShareContentUri = contentUri;
        qDebug() << "Content sharing URI:" << contentUri << "share mode:" << mShareMode;
    }

    if (!mSharingInProgress)
        return;

    if (!contentUri.isNull())
        shareContent();
    else
        qDebug() << "No content URI for sharing";

    setSharingInProgress(false);
}

void SpiralScene::setSharingInProgress(bool inProgress)
{
    if (inProgress != mSharingInProgress)
    {
        qDebug() << "Sharing in progress:" << inProgress;
        mSharingInProgress = inProgress;
        emit sharingInProgressChanged();
    }
}

void SpiralScene::handleReceivedAndroidIntent(const QString& uri)
{
    stop();

    SpiralConfig cfg(mCircles, mDefaultCircleRadius);
    try {
        CircleConfigList circleCfg = cfg.decodeConfigAppUri(uri);
        if (!circleCfg.empty())
        {
            setupCircles(circleCfg);
        }
    } catch (RuntimeException& e) {
        emit message(QString("Broken link: %1").arg(e.msg()));
    }
}

void SpiralScene::saveConfig()
{
    const QSize imageSize = size().scaled(CFG_IMAGE_SIZE, CFG_IMAGE_SIZE, Qt::KeepAspectRatioByExpanding).toSize();
    auto grabResult = grabToImage(imageSize);
    if (!grabResult)
    {
        emit message("Failed to grab image.");
        return;
    }

    QObject::connect(grabResult.get(), &QQuickItemGrabResult::ready, this,
        [this, grabResult]{
            const QImage img = grabResult->image();
            const QImage thumbnail = Utils::createThumbnail(img, size(), mSceneRect, CFG_IMAGE_SIZE);
            SpiralConfig cfg(mCircles, mDefaultCircleRadius);

            try {
                cfg.save(thumbnail);
                emit statusUpdate("Config saved");
            } catch (RuntimeException& e) {
                emit message(e.msg());
            }
        });
}

QObjectList SpiralScene::getConfigFileList()
{
    mConfigFileList.clear();
    SpiralConfig cfg(mCircles, mDefaultCircleRadius);

    QObjectList l;
    try {
        l = cfg.getConfigFiles();
    } catch (RuntimeException& e) {
        emit message(e.msg());
        return {};
    }

    // The listview in QML needs raw pointers.
    // Wrap them in unique pointers so we can cleanup next time.
    for (auto obj : l)
        mConfigFileList.emplace_back(obj);

    return l;
}

void SpiralScene::loadConfig(const QString& fileName)
{
    SpiralConfig cfg(mCircles, mDefaultCircleRadius);

    try {
        const CircleConfigList circleCfgList = cfg.load(fileName);
        if (!circleCfgList.empty())
            setupCircles(circleCfgList);
    } catch (RuntimeException& e) {
        emit message(e.msg());
    }
}

void SpiralScene::deleteConfig(const QStringList& fileNameList)
{
    SpiralConfig cfg(mCircles, mDefaultCircleRadius);
    cfg.remove(fileNameList);
}

QStringList SpiralScene::getCircleColorList() const
{
    QStringList colorList;
    for (const auto& c : mCircles)
    {
        const QString colorName = c->getColor().name();
        colorList.push_back(colorName);
    }

    return colorList;
}

QRectF SpiralScene::getMaxSceneRect() const
{
    Q_ASSERT(!mCircles.empty());
    qreal halfSize = mCircles[0]->getRadius();

    for (unsigned i = 1; i < mCircles.size(); ++i)
        halfSize += mCircles[i]->getDiameter();

    const QPointF& center = mCircles[0]->getCenter();
    const qreal x = center.x() - halfSize;
    const qreal y = center.y() - halfSize;

    return QRectF(x, y, halfSize * 2, halfSize * 2);
}

}

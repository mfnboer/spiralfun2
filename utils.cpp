// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "utils.h"
#include "exception.h"
#include <QDir>
#include <QJniObject>
#include <QStandardPaths>

#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#else
#include "jni_callback.h"
#endif

namespace {
constexpr const char* PICTURES_SUB_DIR = "SpiralFun";
constexpr const char* SPIRAL_CONFIG_SUB_DIR = "SpiralFun/SpiralConfigs";

bool checkStoragePermission()
{
#if defined(Q_OS_ANDROID)
    auto checkResult = QtAndroidPrivate::checkPermission(QtAndroidPrivate::Storage);
    if (checkResult.result() != QtAndroidPrivate::Authorized)
    {
        qDebug() << "Write storage permission check failed.";
        auto requestResult = QtAndroidPrivate::requestPermission(QtAndroidPrivate::Storage);
        if (requestResult.result() != QtAndroidPrivate::Authorized)
        {
            qWarning() << "No permission to write to storage.";
            return false;
        }
    }
#endif
    return true;
}

}

namespace SpiralFun::Utils {

QString getPicturesPath()
{
    if (!checkStoragePermission())
        throw RuntimeException("No permission to access storage.");

#if defined(Q_OS_ANDROID)
    auto jsSubDir = QJniObject::fromString(PICTURES_SUB_DIR);
    auto pathObj = QJniObject::callStaticMethod<jstring>("com/gmail/mfnboer/QAndroidUtils",
                                                         "getPicturesPath",
                                                         "(Ljava/lang/String;)Ljava/lang/String;",
                                                         jsSubDir.object<jstring>());
    if (!pathObj.isValid())
        throw RuntimeException("Cannot create pictures storage path.");

    const QString picPath = pathObj.toString();
    qDebug() << "Pictures path:" << picPath;
#else
    auto path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    const QString picPath = path + "/" + PICTURES_SUB_DIR;

    if (!QDir().mkpath(picPath))
        throw RuntimeException(QString("Failed to create path: %1").arg(picPath));
#endif

    return picPath;
}

QString getPublicSpiralConfigPath()
{
    if (!checkStoragePermission())
        throw RuntimeException("No permission to access storage.");

#if defined(Q_OS_ANDROID)
    auto jsSubDir = QJniObject::fromString(SPIRAL_CONFIG_SUB_DIR);
    auto pathObj = QJniObject::callStaticMethod<jstring>("com/gmail/mfnboer/QAndroidUtils",
                                                         "getPublicSpiralConfigPath",
                                                         "(Ljava/lang/String;)Ljava/lang/String;",
                                                         jsSubDir.object<jstring>());
    if (!pathObj.isValid())
        throw RuntimeException("Cannot create public spiral config storage path.");

    const QString cfgPath = pathObj.toString();
    qDebug() << "Pictures path:" << cfgPath;
#else
    const QString cfgPath;
#endif

    return cfgPath;
}

QString getSpiralConfigPath()
{
#if defined(Q_OS_ANDROID)
    auto jsSubDir = QJniObject::fromString(SPIRAL_CONFIG_SUB_DIR);
    auto pathObj = QJniObject::callStaticMethod<jstring>("com/gmail/mfnboer/QAndroidUtils",
                                                         "getSpiralConfigPath",
                                                         "(Ljava/lang/String;)Ljava/lang/String;",
                                                         jsSubDir.object<jstring>());
    if (!pathObj.isValid())
        throw RuntimeException("Cannot create spiral config storage path.");

    const QString cfgPath = pathObj.toString();
    qDebug() << "Spiral config path:" << cfgPath;
#else
    auto path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString cfgPath = path + "/" + SPIRAL_CONFIG_SUB_DIR;
    if (!QDir().mkpath(cfgPath))
        throw RuntimeException(QString("Failed to create path: %1").arg(cfgPath));
#endif

    return cfgPath;
}

QString createDateTimeName()
{
    return QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
}

QString createPictureFileName(bool forSharing)
{
    return forSharing ? "_TMP_SHARE.jpg" : QString("IMG_%1.jpg").arg(createDateTimeName());
}

void scanMediaFile(const QString& fileName)
{
#if defined(Q_OS_ANDROID)
    auto jsFileName = QJniObject::fromString(fileName);
    QJniObject::callStaticMethod<void>("com/gmail/mfnboer/QAndroidUtils",
                                       "scanMediaFile",
                                       "(Ljava/lang/String;)V",
                                       jsFileName.object<jstring>());
#else
    qDebug() << "No need to scan media:" << fileName;
    JNICallbackListener::getInstance().handleMediaScannerFinished({});
#endif
}

void sharePicture(const QString& contentUri, const QString& configAppUri)
{
#if defined(Q_OS_ANDROID)
    auto jsContentUri = QJniObject::fromString(contentUri);
    auto jsConfigAppUri = QJniObject::fromString(configAppUri);
    QJniObject::callStaticMethod<void>("com/gmail/mfnboer/QAndroidUtils",
                                       "sharePicture",
                                       "(Ljava/lang/String;Ljava/lang/String;)V",
                                       jsContentUri.object<jstring>(),
                                       jsConfigAppUri.object<jstring>());
#else
    qDebug() << "Sharing not supported, content:" << contentUri;
    qDebug() << "config:" << configAppUri;
#endif
}

void handlePendingIntent()
{
#if defined(Q_OS_ANDROID)
    if (!QNativeInterface::QAndroidApplication::isActivityContext())
    {
        qWarning() << "Cannot find Android activity";
        return;
    }

    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    activity.callMethod<void>("handlePendingIntent");
#endif
}

QImage createThumbnail(const QImage& scaledImg, const QSizeF& origSize, const QRectF& sceneRect, int thumbnailSize)
{
    const QSize sz = scaledImg.size();
    QPointF center = sceneRect.center();
    center.rx() *= qreal(sz.width()) / origSize.width();
    center.ry() *= qreal(sz.height()) / origSize.height();
    const int x = std::max(center.x() - thumbnailSize / 2.0, 0.0);
    const int y = std::max(center.y() - thumbnailSize / 2.0, 0.0);
    const QImage thumbnail = scaledImg.copy(x, y, thumbnailSize, thumbnailSize);
    return thumbnail;
}

QImage extractSpiral(const QImage& grabbedImg, const QRectF& sceneRect, int margin, qreal devicePixelRatio)
{
    QRectF cutRect = sceneRect.adjusted(-margin, -margin, margin, margin);
    cutRect = QRectF(cutRect.topLeft() * devicePixelRatio, cutRect.size() * devicePixelRatio);
    const QImage spiral = grabbedImg.copy(cutRect.toRect());
    return spiral;
}

}

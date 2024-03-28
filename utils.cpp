// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "utils.h"
#include "exception.h"
#include <QDir>
#include <QStandardPaths>

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QOperatingSystemVersion>
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
    static const QString WRITE_EXTERNAL_STORAGE = "android.permission.WRITE_EXTERNAL_STORAGE";

    const auto osVersion = QOperatingSystemVersion::current();

    if (osVersion >= QOperatingSystemVersion::Android11)
        return true;


    auto checkResult = QtAndroidPrivate::checkPermission(WRITE_EXTERNAL_STORAGE);
    if (checkResult.result() != QtAndroidPrivate::Authorized)
    {
        qDebug() << "Write storage permission check failed.";
        auto requestResult = QtAndroidPrivate::requestPermission(WRITE_EXTERNAL_STORAGE);
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

QString getPicturesPath(const QString& subDir)
{
    if (!checkStoragePermission())
        throw RuntimeException("No permission to access storage.");

#if defined(Q_OS_ANDROID)
    QString picSubPath = PICTURES_SUB_DIR;
    if (!subDir.isEmpty())
        picSubPath += "/" + subDir;

    auto jsSubDir = QJniObject::fromString(picSubPath);
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
    QString picPath = path + "/" + PICTURES_SUB_DIR;
    if (!subDir.isEmpty())
        picPath += "/" + subDir;

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

QString createPictureFileName(const QString& baseNameSuffix)
{
    return QString("IMG_%1%2.jpg").arg(createDateTimeName(), baseNameSuffix);
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

void shareMedia(const QString& contentUri, const QString& configAppUri, const QString& mimeType)
{
#if defined(Q_OS_ANDROID)
    auto jsContentUri = QJniObject::fromString(contentUri);
    auto jsConfigAppUri = QJniObject::fromString(configAppUri);
    auto jsMimeType = QJniObject::fromString(mimeType);
    QJniObject::callStaticMethod<void>("com/gmail/mfnboer/QAndroidUtils",
                                       "shareMedia",
                                       "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
                                       jsContentUri.object<jstring>(),
                                       jsConfigAppUri.object<jstring>(),
                                       jsMimeType.object<jstring>());
#else
    qDebug() << "Sharing not supported, content:" << contentUri << "mimetype:" << mimeType;
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

void setKeepScreenOn(bool keepOn)
{
#if defined(Q_OS_ANDROID)
    if (!QNativeInterface::QAndroidApplication::isActivityContext())
    {
        qWarning() << "Cannot find Android activity";
        return;
    }

    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    activity.callMethod<void>("setKeepScreenOn", "(Z)V", (jboolean)keepOn);
#else
    Q_UNUSED(keepOn);
#endif
}

bool sendAppToBackground()
{
#ifdef Q_OS_ANDROID
    if (!QNativeInterface::QAndroidApplication::isActivityContext())
    {
        qWarning() << "Cannot find Android activity";
        return false;
    }

    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    activity.callMethod<void>("goToBack", "()V");
    return true;
#else
    return false;
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

}

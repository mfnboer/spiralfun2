// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "utils.h"
#include <QDir>
#include <QJniObject>
#include <QStandardPaths>

#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
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
        return QString();

#if defined(Q_OS_ANDROID)
    auto jsSubDir = QJniObject::fromString(PICTURES_SUB_DIR);
    auto pathObj = QJniObject::callStaticMethod<jstring>("com/gmail/mfnboer/QAndroidUtils",
                                                         "getPicturesPath",
                                                         "(Ljava/lang/String;)Ljava/lang/String;",
                                                         jsSubDir.object<jstring>());
    if (!pathObj.isValid())
    {
        qWarning() << "Cannot create pictures path.";
        return QString();
    }

    const QString picPath = pathObj.toString();
    qDebug() << "Pictures path:" << picPath;
#else
    auto path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    const QString picPath = path + "/" + PICTURES_SUB_DIR;
    if (!QDir().mkpath(picPath))
    {
        qWarning() << "Failed to create path:" << picPath;
        return QString();
    }
#endif

    return picPath;
}

QString getSpiralCongifPath()
{    if (!checkStoragePermission())
        return QString();

#if defined(Q_OS_ANDROID)
    auto jsSubDir = QJniObject::fromString(SPIRAL_CONFIG_SUB_DIR);
    auto pathObj = QJniObject::callStaticMethod<jstring>("com/gmail/mfnboer/QAndroidUtils",
                                                         "getSpiralConfigPath",
                                                         "(Ljava/lang/String;)Ljava/lang/String;",
                                                         jsSubDir.object<jstring>());
    if (!pathObj.isValid())
    {
        qWarning() << "Cannot create spiral config path.";
        return QString();
    }

    const QString cfgPath = pathObj.toString();
    qDebug() << "Spiral config path:" << cfgPath;

    // The .nomedia file indicates that the images in the directoy should not
    // be picked up by the media scanner
    QFile nomedia(cfgPath + "/.nomedia");
    if (!nomedia.exists())
        nomedia.open(QIODevice::WriteOnly);
#else
    auto path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString cfgPath = path + "/" + SPIRAL_CONFIG_SUB_DIR;
    if (!QDir().mkpath(cfgPath))
    {
        qWarning() << "Failed to create path:" << cfgPath;
        return QString();
    }
#endif

    return cfgPath;
}

QString createDateTimeName()
{
    return QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
}

QString createPictureFileName()
{
    return QString("IMG_%1.jpg").arg(createDateTimeName());
}

void scanMediaFile(const QString& fileName, bool share)
{
#if defined(Q_OS_ANDROID)
    auto jsFileName = QJniObject::fromString(fileName);
    jboolean jsShare = share;
    QJniObject::callStaticMethod<void>("com/gmail/mfnboer/QAndroidUtils",
                                       "scanMediaFile",
                                       "(Ljava/lang/String;Z)V",
                                       jsFileName.object<jstring>(), jsShare);
#else
    if (share)
    {
        qWarning() << "Sharing not supported:" << fileName;
    }
#endif
}

}

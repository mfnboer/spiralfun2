// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "file_utils.h"
#include <QDir>
#include <QJniObject>
#include <QStandardPaths>

#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif

namespace SpiralFun {

QString getPicturesPath()
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
            return QString();
        }
    }

    auto pathObj = QJniObject::callStaticMethod<jstring>("com/gmail/mfnboer/QAndroidUtils",
                                                         "getPicturesPath");
    if (!pathObj.isValid())
    {
        qWarning() << "Cannot create pictures path.";
        return QString();
    }

    const QString picPath = pathObj.toString();
    qDebug() << "Pictures path:" << picPath;
#else
    auto path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    const QString picPath = path + "/SpiralFun";
    if (!QDir().mkpath(picPath))
    {
        qWarning() << "Failed to create path." << picPath;
        return QString();
    }
#endif

    return picPath;
}

QString createPictureFileName()
{
    return QString("IMG_%1.jpg").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
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
        qWarning() << "Shareing not supported:" << fileName;
    }
#endif
}

}

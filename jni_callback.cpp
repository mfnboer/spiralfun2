// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "jni_callback.h"
#include <QDebug>
#include <QJniObject>

namespace SpiralFun {

Q_GLOBAL_STATIC(std::unique_ptr<JNICallbackListener>, gTheInstance);

namespace {
#if defined(Q_OS_ANDROID)
void _handleViewUriReceived(JNIEnv* env, jobject, jstring jsUri)
{
    QString uri(env->GetStringUTFChars(jsUri, nullptr));
    qDebug() << "Received URI:" << uri;

    auto& instance = *gTheInstance;
    if (instance)
        instance->handleViewUriReceived(uri);
}

void _handleMediaScannerFinished(JNIEnv* env, jobject, jstring jsUri)
{
    QString uri = jsUri ? env->GetStringUTFChars(jsUri, nullptr) : QString();
    qDebug() << "Media scanner finished:" << uri;
    auto& instance = *gTheInstance;
    if (instance)
        instance->handleMediaScannerFinished(uri);
}
#endif
}

JNICallbackListener& JNICallbackListener::getInstance()
{
    auto& instance = *gTheInstance;
    if (!instance)
        instance.reset(new JNICallbackListener);

    return *instance;
}

JNICallbackListener::JNICallbackListener() : QObject()
{
#if defined(Q_OS_ANDROID)
    QJniEnvironment jni;

    const JNINativeMethod spiralFunActivityCallbacks[] = {
        { "emitViewUriReceived", "(Ljava/lang/String;)V", reinterpret_cast<void *>(_handleViewUriReceived) }
    };
    jni.registerNativeMethods("com/gmail/mfnboer/QSpiralFunActivity", spiralFunActivityCallbacks, 1);

    const JNINativeMethod androidUtilsCallbacks[] = {
        { "emitMediaScannerFinished", "(Ljava/lang/String;)V", reinterpret_cast<void *>(_handleMediaScannerFinished) }
    };
    jni.registerNativeMethods("com/gmail/mfnboer/QAndroidUtils", androidUtilsCallbacks, 1);
#endif
}

void JNICallbackListener::handleViewUriReceived(const QString& uri)
{
    emit viewUriReceived(uri);
}

void JNICallbackListener::handleMediaScannerFinished(const QString contentUri)
{
    emit mediaScannerFinished(contentUri);
}

}

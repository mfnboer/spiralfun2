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
    const JNINativeMethod callbacks[] = {
        { "emitViewUriReceived", "(Ljava/lang/String;)V", reinterpret_cast<void *>(_handleViewUriReceived)}
    };
    QJniEnvironment jni;
    jni.registerNativeMethods("com/gmail/mfnboer/QSpiralFunActivity", callbacks, 1);
#endif
}

void JNICallbackListener::handleViewUriReceived(const QString& uri)
{
    emit viewUriReceived(uri);
}

}

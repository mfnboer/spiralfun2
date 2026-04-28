// Copyright (C) 2025 Michel de Boer
// License: GPLv3
#include "display_utils.h"
#include "utils.h"

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>
#endif

namespace SpiralFun {

DisplayUtils::DisplayUtils(QObject* parent) : QObject(parent)
{
}

int DisplayUtils::getNavigationBarSize(QEnums::InsetsSide side)
{
    return Utils::getNavigationBarSize(side);
}

int DisplayUtils::getStatusBarSize(QEnums::InsetsSide side)
{
    return Utils::getStatusBarSize(side);
}

int DisplayUtils::getDisplayCutoutSize(QEnums::InsetsSide side)
{
    return Utils::getDisplayCutoutSize(side);
}

void DisplayUtils::openLinkInApp(const QString& link)
{
#ifdef Q_OS_ANDROID
    if (!QNativeInterface::QAndroidApplication::isActivityContext())
    {
        qWarning() << "Cannot find Android activity";
        return;
    }

    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    QJniObject jLink = QJniObject::fromString(link);

    activity.callMethod<void>(
        "openLinkInApp",
        "(Ljava/lang/String;)V",
        jLink.object<jstring>());
#else
    qWarning() << "Cannot open link in app:" << link;
#endif
}

}

// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QImage>
#include <QString>

namespace SpiralFun::Utils {

QString createDateTimeName();
QString getPicturesPath(const QString& subDir = "");
QString getPublicSpiralConfigPath();
QString getSpiralConfigPath();
QString createPictureFileName(const QString& baseNameSuffix = "");
void scanMediaFile(const QString& fileName);
void shareMedia(const QString& contentUri, const QString& configAppUri, const QString& mimeType);
void handlePendingIntent();
void setKeepScreenOn(bool keepOn);
bool sendAppToBackground();

QImage createThumbnail(const QImage& scaledImg, const QSizeF& origSize, const QRectF& sceneRect, int thumbnailSize);

}

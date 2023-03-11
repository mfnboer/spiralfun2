// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QImage>
#include <QString>

namespace SpiralFun::Utils {

QString createDateTimeName();
QString getPicturesPath();
QString getPublicSpiralConfigPath();
QString getSpiralConfigPath();
QString createPictureFileName(bool forSharing);
void scanMediaFile(const QString& fileName);
void sharePicture(const QString& contentUri, const QString& configAppUri);
void handlePendingIntent();

QImage createThumbnail(const QImage& scaledImg, const QSizeF& origSize, const QRectF& sceneRect, int thumbnailSize);
QImage extractSpiral(const QImage& grabbedImg, const QRectF& sceneRect, int margin, qreal devicePixelRatio);

}

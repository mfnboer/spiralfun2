// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QImage>
#include <QString>

namespace SpiralFun::Utils {

QString createDateTimeName();
QString getPicturesPath();
QString getSpiralCongifPath();
QString createPictureFileName();
void scanMediaFile(const QString& fileName, bool share);
QImage createThumbnail(const QImage& scaledImg, const QSizeF& origSize, const QRectF& sceneRect, int thumbnailSize);
QImage extractSpiral(const QImage& grabbedImg, const QRectF& sceneRect, int margin, qreal devicePixelRatio);

}

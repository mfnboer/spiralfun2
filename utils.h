// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QString>

namespace SpiralFun::Utils {

QString getPicturesPath();
QString createPictureFileName();
void scanMediaFile(const QString& fileName, bool share);

}

// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include <QPushButton>

namespace SpiralFun {

class ImageButton : public QPushButton
{
public:
    ImageButton(const QString& fileName, QWidget* parent = nullptr) :
        QPushButton(parent)
    {
        QPixmap image(fileName);
        setIcon(image);
        setIconSize(image.size());
        setFlat(true);
    }
};

}

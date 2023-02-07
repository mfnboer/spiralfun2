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

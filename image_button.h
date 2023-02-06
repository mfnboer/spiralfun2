#pragma once

#include <QPushButton>

namespace SpiralFun {

class ImageButton : public QPushButton
{
public:
    ImageButton(const QString& fileName, const QSize& size = QSize(150, 150), QWidget* parent = nullptr) :
        QPushButton(parent)
    {
        QPixmap image = QPixmap(fileName).scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        setIcon(image);
        setIconSize(size);
    }
};

}

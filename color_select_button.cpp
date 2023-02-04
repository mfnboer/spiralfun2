#include "color_select_button.h"

#include <QColorDialog>

namespace SpiralFun {

ColorSelectButton::ColorSelectButton(const QColor& color, QWidget* parent) :
    QPushButton(parent),
    mColor(color)
{
    updateButtonColor();
    QObject::connect(this, &QPushButton::clicked, this, &ColorSelectButton::selectColor);
}

void ColorSelectButton::setColor(const QColor& color)
{
    mColor = color;
    updateButtonColor();
    emit colorChanged(mColor);
}

void ColorSelectButton::updateButtonColor()
{
    setStyleSheet("background-color: " + mColor.name());
}

void ColorSelectButton::selectColor()
{
    QColor color = QColorDialog::getColor(mColor, parentWidget(), "", QColorDialog::DontUseNativeDialog);
    if (!color.isValid() || color == mColor)
        return;

    setColor(color);
}

}

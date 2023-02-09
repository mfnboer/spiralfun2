// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include <QPushButton>

namespace SpiralFun {

class ColorSelectButton : public QPushButton
{
    Q_OBJECT

public:
    ColorSelectButton(const QColor& color = Qt::white, QWidget* parent = nullptr);

    void setColor(const QColor& color);
    const QColor& getColor() const { return mColor; }

signals:
    void colorChanged(const QColor& color);

private:
    void updateButtonColor();
    void selectColor();

    QColor mColor;
};

}

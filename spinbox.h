#pragma once

#include <QGuiApplication>
#include <QSpinBox>

namespace SpiralFun {

class SpinBox : public QSpinBox
{
public:
    SpinBox(QWidget* parent = nullptr) : QSpinBox(parent)
    {
        // This handles the 'Go' button on the numeric keypad on Android.
        // It makes the keypad go away and avoids auto-selection of the text.
        QObject::connect(this, &QSpinBox::editingFinished, this, [this]{
                         QGuiApplication::inputMethod()->hide();
                         clearFocus();
        });
    }
};

}

#pragma once

#include <QGuiApplication>
#include <QSpinBox>

namespace SpiralFun {

class SpinBox : public QSpinBox
{
public:
    SpinBox(QWidget *parent = nullptr) : QSpinBox(parent)
    {
        QObject::connect(this, &QSpinBox::editingFinished, this, [this]{
                         QGuiApplication::inputMethod()->hide();
                         clearFocus();
        });
    }
};

}

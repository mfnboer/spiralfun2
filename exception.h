// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include <QException>

namespace SpiralFun {

class RuntimeException : public QException
{
public:
    explicit RuntimeException(const QString& msg) : mMsg(msg) {}

    const QString& msg() const { return mMsg; }
    void raise() const override { throw *this; }
    RuntimeException *clone() const override { return new RuntimeException(*this); }

private:
    QString mMsg;
};

}

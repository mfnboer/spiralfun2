// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once

#include "circle.h"
#include <qqml.h>
#include <QObject>

namespace SpiralFun {

class Mutation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int circle READ getCircle WRITE setCircle NOTIFY circleChanged)
    Q_PROPERTY(Mutation::Trait trait READ getTrait WRITE setTrait NOTIFY traitChanged)
    Q_PROPERTY(Mutation::Change change READ getChange WRITE setChange NOTIFY changeChanged)
    QML_ELEMENT

public:
    enum Trait { TRAIT_DIAMETER, TRAIT_ROTATIONS, TRAIT_DIRECTION };
    Q_ENUM(Trait);

    enum Change { CHANGE_INCREMENT, CHANGE_DECREMENT };
    Q_ENUM(Change);

    int getCircle() const { return mCircle; }
    Trait getTrait() const { return mTrait; }
    Change getChange() const { return mChange; }

    void setCircle(int circle) { mCircle = circle; emit circleChanged(); }
    void setTrait(Trait trait) { mTrait = trait; emit traitChanged(); }
    void setChange(Change change) { mChange = change; emit changeChanged(); }

    void init(const CircleList& circleList);
    void apply(const CircleList& circleList, int maxDiameter, bool reverse = false) const;

signals:
    void circleChanged();
    void traitChanged();
    void changeChanged();

private:
    int mCircle = 0;
    Trait mTrait = TRAIT_ROTATIONS;
    Change mChange = CHANGE_INCREMENT;
    int mRotationDeltaFactor = 1;
};

class MutationFactory : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    Q_INVOKABLE SpiralFun::Mutation* createMutation(int circle);
};

}

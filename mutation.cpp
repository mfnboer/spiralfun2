// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "mutation.h"

namespace SpiralFun {

void Mutation::init(const CircleList& circleList)
{
    auto& circle = circleList[getCircle()];
    mRotationDeltaFactor = circle->getSpeed() < 0 ? -1 : 1;
}

void Mutation::apply(const CircleList& circleList) const
{
    auto& circle = circleList[getCircle()];

    switch (getTrait())
    {
    case Mutation::TRAIT_ROTATIONS: {
            const int delta = (getChange() == Mutation::CHANGE_INCREMENT ? 1 : -1) * mRotationDeltaFactor;
            const int speed = circle->getSpeed() + delta;
            circle->setSpeed(speed);
            break;
        }
    case Mutation::TRAIT_DIAMETER: {
            const int delta = getChange() == Mutation::CHANGE_INCREMENT ? 1 : -1;
            const int diameter = std::max(circle->getDiameter() + delta, 1);
            circle->setDiameter(diameter);
            break;
        }
    case Mutation::TRAIT_DIRECTION: {
            const Circle::Direction direction = circle->getDirection() == Circle::CLOCKWISE ?
                        Circle::COUNTER_CLOCKWISE : Circle::CLOCKWISE;
            circle->setDirection(direction);
            break;
        }
    }
}

}

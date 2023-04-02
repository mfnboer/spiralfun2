// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "mutation.h"
#include "spiral_config.h"
#include <QQmlEngine>

namespace SpiralFun {

void Mutation::init(const CircleList& circleList)
{
    auto& circle = circleList[getCircle()];
    mRotationDeltaFactor = circle->getSpeed() < 0 ? -1 : 1;
}

void Mutation::apply(const CircleList& circleList, int maxDiameter, bool reverse) const
{
    auto& circle = circleList[getCircle()];
    const int changeFactor = reverse ? -1 : 1;

    switch (getTrait())
    {
    case Mutation::TRAIT_ROTATIONS: {
            const int delta = (getChange() == Mutation::CHANGE_INCREMENT ? 1 : -1) * mRotationDeltaFactor * changeFactor;
            const int newSpeed = circle->getSpeed() + delta;
            const int speed = std::clamp(newSpeed, -SpiralConfig::MAX_SPEED, SpiralConfig::MAX_SPEED);
            circle->setSpeed(speed);
            break;
        }
    case Mutation::TRAIT_DIAMETER: {
            const int delta = (getChange() == Mutation::CHANGE_INCREMENT ? 1 : -1) * changeFactor;
            const int newDiameter = circle->getDiameter() + delta;
            const int diameter = std::clamp(newDiameter, 1, maxDiameter);
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

Mutation* MutationFactory::createMutation(int circle)
{
    auto* mutation = new Mutation;
    mutation->setParent(this);
    auto* engine = qmlEngine(this);
    engine->setContextForObject(mutation, qmlContext(this));
    mutation->setCircle(circle);
    return mutation;
}

}

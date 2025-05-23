﻿#include "../include/SpaceObject.h"

std::string SpaceObject::getName() const
{
    return positionable::toCharId(this->globalObjectId);
}

unsigned int SpaceObject::nextId = 1000;

SpaceObject SpaceObject::objectAt(double time) const
{
    SpaceObject out(*this);
    out.getCurrentPosObj() = this->posObj.objectAt(time);
    out.lifetime = lifeTime{this->lifetime.start - time, this->lifetime.end - time};
    return out;
}

SpaceObject SpaceObject::update(double deltaTime) const
{
    auto out(*this);
    return out;
}

std::ostream& operator<<(std::ostream& os, const SpaceObject& obj)
{
    os << obj.getName() << "(" << obj.meta << ")";
    return os;
}

positionable SpaceObject::getCurrentPosObj() const {
    return posObj;
}

#pragma once
#include <functional>
#include <string>
#include <memory>

#include "ObjectMetaInfo.h"
#include "positionable.h"
#include "Updateable.h"

class Updateable;
class WorldState;
class SpaceObject;

namespace sf
{
    class Color;
}

class SpaceObject
{
public:
    // Copy constructor
    SpaceObject(const SpaceObject& space_object)
        : positionable(space_object.positionable),
          lifetime(space_object.lifetime),
          meta(space_object.meta),
          globalObjectId(space_object.globalObjectId),
          updater(space_object.updater != nullptr ? space_object.updater->clone() : nullptr)
    {
        // No need for additional code here
    }

    static unsigned int nextId;

    // Parameterized constructor
    explicit SpaceObject(positionable p)
        : positionable(p), updater(nullptr)
    {
        // No need for additional code here
    }

    // Parameterized constructor
    SpaceObject(positionable p, lifeTime k, ObjectMetaInfo m, unsigned int globalId)
        : positionable(p),
          lifetime(k),
          meta(m),
          globalObjectId(globalId),
          updater(nullptr)
    {
        // No need for additional code here
    }

    ~SpaceObject()
    {
    }

    SpaceObject& operator=(const SpaceObject& other)
    {
        if (this != &other)
        {
            this->positionable = other.positionable;
            this->lifetime = other.lifetime;
            this->meta = other.meta;
            this->globalObjectId = other.globalObjectId;
            this->updater = other.updater == nullptr ? nullptr : other.updater->clone();
        }
        return *this;
    }

    std::string getName() const;
    positionable positionable;
    lifeTime lifetime;
    ObjectMetaInfo meta;
    unsigned int globalObjectId;
    std::unique_ptr<Updateable> updater;

    using UpdateFunction = std::function<void(SpaceObject&, double)>;

    /** extrapolates object members to time. only math, no logic.
     * \brief 
     * \param time 
     * \return 
     */
    SpaceObject objectAt(double time) const;

    /** call object internal update to achieve things like changing course
     * \brief 
     * \param deltaTime 
     * \return 
     */
    SpaceObject update(double deltaTime) const;

    // Overload << operator for positionable
    friend std::ostream& operator<<(std::ostream& os, const SpaceObject& obj);
};

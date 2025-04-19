#pragma once
#include <memory>

class WorldState;
class SpaceObject;

class Updateable
{
public:
    virtual std::unique_ptr<Updateable> clone() const = 0; // Pure virtual clone method
    virtual void onUpdate(const SpaceObject& obj, const WorldState* const currentState, WorldState* outState,
                          double currentTime, double updateDelta)
    {
    }
};

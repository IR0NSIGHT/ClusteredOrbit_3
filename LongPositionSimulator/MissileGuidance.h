#pragma once
#include "Updateable.h"

class SpaceObject;
class WorldState;

class MissileGuidance : public Updateable
{
public:
    MissileGuidance(unsigned int targetId, double thrust) : targetGlobalId(targetId), thrust(thrust)
    {
    };

    MissileGuidance(const MissileGuidance* guidance) : targetGlobalId(guidance->targetGlobalId),
                                                       thrust(guidance->thrust)
    {
    };

    std::unique_ptr<Updateable> clone() const override
    {
        return std::unique_ptr<Updateable>(new MissileGuidance(this));
    }; // Pure virtual clone method

    unsigned int targetGlobalId;
    double thrust;
    void onUpdate(const SpaceObject& obj, const WorldState* const currentState, WorldState* outState,
                  double currentTime, double updateDelta) override;
};

#pragma once
#include "ObjectMetaInfo.h"
#include "Updateable.h"

class PointDefenseAI : public Updateable
{
private:
    ObjectMetaInfo meta;

public:
    explicit PointDefenseAI(const ObjectMetaInfo& meta)
        : meta(meta)
    {
    }

    std::unique_ptr<Updateable> clone() const override;
    void onUpdate(const SpaceObject& thisShip, const WorldState* const currentState, WorldState* outState,
                  double currentTime, double updateDelta) override;
};

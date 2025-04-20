#pragma once

#include <algorithm>
#include <set>
#include <vector>

#include "BasicPuppetMaster.h"
#include "Collission.h"
#include "SpaceObject.h"

// positionable struct
class WorldState
{
public:
    explicit WorldState(unsigned int size)
    {
        objects.reserve(size);
    }

    ~WorldState()
    {
    }

    bool containsObject(unsigned int globalId) const;
    void putObject(SpaceObject obj, double currentTime);

    unsigned int getSize() const;

    // Deep copy of WorldState
    WorldState clone() const;

    /**
     * Creates a new WorldState
     */
    std::shared_ptr<WorldState> update(double globalMillis, double deltaMillis) const;

    void calculateCollissions(double currentTime);

    void setObject(SpaceObject object);

    const std::optional<SpaceObject> getObject(unsigned int globalId, double currentTime) const;

    void printObjects() const;

    std::vector<SpaceObject> aliveObjectsAt(double time) const;
    BasicPuppetMaster* eventHandler;

    std::unordered_map<unsigned int, unsigned int> globalIdToInternalId;
    std::vector<SpaceObject> objects;
    // all collissions between objects, sorted in ascending time order.
    std::vector<CollissionPoint> collission_points_;

    std::vector<CollissionPoint> findHappenedCollissionsBetween(double start, double end,
                                                                std::set<unsigned int>& collidedObjects) const;

    bool invariant() const;
};

#include "WorldState.h"

#include <cassert>
#include <iostream>
#include <set>
#include <SFML/Window/WindowEnums.hpp>

#include "shipIds.h"

// vel3d implementation
std::ostream& operator<<(std::ostream& os, const vel3d& vel)
{
    os << "vel3d(" << vel.x << ", " << vel.y << ", " << vel.z << ")";
    return os;
}

// positionable implementation
std::ostream& operator<<(std::ostream& os, const positionable& obj)
{
    os << "obj(s=" << obj.position << ", v=" << obj.velocity << ", a=" << obj.acceleration << ", r=" << obj.radius <<
        ")";
    return os;
}

unsigned int WorldState::getSize() const
{
    return objects.size();
}


/**
 * Creates a new WorldState
 */
std::shared_ptr<WorldState> WorldState::update(double globalMillis, double deltaMillis) const
{
    assert(invariant());
    assert(this->eventHandler);

    double lastUpdateGlobalMillis = globalMillis - deltaMillis;
    auto tempNewState = new WorldState(10); //eventhandler will write changes into this state
    eventHandler->setUp(this, tempNewState);
    eventHandler->setTotalTime(globalMillis);

    //objects that changed course last update get events
    for (auto changed : eventHandler->changedObjects)
    {
        eventHandler->onObjectChangedCourse(changed.oldObj, changed.newObj);
    }
    eventHandler->changedObjects.clear();

    std::set<unsigned int> collided;
    auto collissions = findHappenedCollissionsBetween(lastUpdateGlobalMillis, globalMillis, collided);
    for (auto& c : collissions)
    {
        eventHandler->onCollissionHappened(c);
    }

    
    auto outState = std::make_shared<WorldState>(objects.size());
    delete outState->eventHandler;
    outState->eventHandler = this->eventHandler;
    for (auto& obj : objects)
    {
        if (collided.find(obj.globalObjectId) != collided.end())
        {
            // remove object
            eventHandler->onDeleteObject(obj);
        }
        else
        {
            if (lastUpdateGlobalMillis < obj.lifetime.start && obj.lifetime.start <= globalMillis)
            {
                eventHandler->onNewObject(obj);
            }
            eventHandler->onUpdate(obj);
            outState->putObject(obj, obj.lifetime.start);    // just copy over, obj didnt change
        }
    }

    //write back changes performed by eventhandler to actual output state
    for (auto obj : tempNewState->objects)
    {
        outState->putObject(obj, globalMillis);
    }

    //compare objects positionable to find changed courses
    for (auto& obj : outState->objects)
    {
        auto& oldObj = getObject(obj.globalObjectId, globalMillis);
        if (oldObj && !oldObj.value().positionable.courseEquals(obj.positionable))
        {
            outState->eventHandler->changedObjects.push_back(ObjectChanged{oldObj.value(), obj});
        }
    }
    outState->calculateCollissions(globalMillis);
    assert(objects.size() != 0);
    return outState;
}

bool WorldState::containsObject(unsigned globalId) const
{
    return globalIdToInternalId.find(globalId) != globalIdToInternalId.end();
}

void WorldState::putObject(SpaceObject obj, double currentTime)
{
    assert(invariant());
    //putting object for this time, means it can not exist earlier.
    obj.lifetime.start = std::max(obj.lifetime.start, currentTime);

    if (containsObject(obj.globalObjectId))
    {    
        //all existing instances of these objs
        for (auto& existing : objects)
        {
            if (existing.globalObjectId == obj.globalObjectId && existing.lifetime.existsAt(obj.lifetime.start))
            {
                assert(existing.meta.type == obj.meta.type);
                //probably an error if two different types of object share the same ID
                // stop existing a tick before new obj starts existing
                existing.lifetime.end = obj.lifetime.start - 0.0001;
                assert(existing.lifetime.end < obj.lifetime.start);
                assert(!existing.lifetime.existsAt(obj.lifetime.start));
                //FIXME do we allow adding in earlier objects?
            }
        }
    }
    // add to hashmap (abused as hashset for now)
    globalIdToInternalId[obj.globalObjectId] = 1;
    objects.push_back(obj);
    assert(invariant());
}

void WorldState::calculateCollissions(double currentTime)
{
    assert(invariant());
    //FIXME
  /*  for (auto& o : objects)
        o.lifetime.end = 100000; // reset
*/
    std::vector<CollissionPoint> collissions;
    collissions.reserve(getSize());
    for (int i = 0; i < objects.size(); i++)
    {
        auto& obj = objects[i];
        for (int j = i + 1; j < objects.size(); j++)
        {
            auto& other = objects[j];
            //test if both exist at some point together
            if (obj.lifetime.start > other.lifetime.end || obj.lifetime.end < other.lifetime.start)
                continue;
            
            assert(obj.globalObjectId != other.globalObjectId);
            //by design two objects are never checked against eachother
            auto coll = Collission::nextCollissionFast(obj, other);
            if (coll)
            {
                //test: objects actually collide at this time.
                assert(
                    coll.value().offsetAtCollission().maxComponentAbs() <= coll.value().obj1.positionable.radius + coll.
                    value().obj2.positionable.radius + 0.1);
                if (obj.lifetime.existsAt(coll.value().time) && other.lifetime.existsAt(coll.value().time))
                {
                    collissions.push_back(coll.value());
                }
            }
        }
    }
    std::sort(collissions.begin(), collissions.end(), [](const CollissionPoint& a, const CollissionPoint& b)
    {
        return a.time < b.time;
    });

    for (auto& coll : collissions)
    {
        auto& o1 = coll.obj1;
        auto& o2 = coll.obj2;

        if (coll.time >= currentTime && o1.lifetime.existsAt(coll.time) && o2.lifetime.existsAt(coll.time))
        {
            if (o1.meta.type != ObjectType::ASTEROID)
                o1.lifetime.end = coll.time;
            if (o2.meta.type != ObjectType::ASTEROID)
                o2.lifetime.end = coll.time;
        }
    }
    this->collission_points_ = collissions;
    assert(invariant());
}

const std::optional<SpaceObject> WorldState::getObject(unsigned int globalId, double currentTime) const
{
    if (!containsObject(globalId))
        return std::nullopt;
    for (const auto& obj : objects)
    {
        if (obj.globalObjectId == globalId && obj.lifetime.existsAt(currentTime))
            return obj;
    }
    return std::nullopt;
}

std::vector<SpaceObject> WorldState::aliveObjectsAt(double deltaMilliseconds) const
{
    // copy over remaining objects
    std::vector<SpaceObject> remainingObjects;
    remainingObjects.reserve(objects.size());
    for (auto obj : objects)
    {
        if (obj.lifetime.start <= deltaMilliseconds && obj.lifetime.end >= deltaMilliseconds)
        {
            auto object = SpaceObject(obj);
            object.positionable = obj.positionable.objectAt(deltaMilliseconds);
            remainingObjects.push_back(object);
        }
    }
    return remainingObjects;
}

std::vector<CollissionPoint> WorldState::findHappenedCollissionsBetween(double start, double end,
                                                                            std::set<unsigned int>& collidedObjects) const
{
    std::vector<CollissionPoint> collissionsThatHappened;
    collissionsThatHappened.reserve(collission_points_.size());
    for (CollissionPoint c : collission_points_)
    {
        assert(c.obj1.globalObjectId != c.obj2.globalObjectId);
        // collissions are sorted by time
        if (start < c.time && c.time <= end && c.obj1.lifetime.existsAt(c.time) && c.obj2.lifetime.existsAt(c.time) &&
            !(collidedObjects.find(c.obj1.globalObjectId) != collidedObjects.end()) &&
            !(collidedObjects.find(c.obj2.globalObjectId) != collidedObjects.end()))
        //collission happened
        {
            collidedObjects.insert(c.obj1.globalObjectId);
            collidedObjects.insert(c.obj2.globalObjectId);
            collissionsThatHappened.push_back(c);
        }
    }
    return collissionsThatHappened;
}

bool WorldState::invariant() const
{
    for (auto& obj : objects)
    {
        if (!containsObject(obj.globalObjectId))
            return false;
    }
    return getSize() == objects.size();
}

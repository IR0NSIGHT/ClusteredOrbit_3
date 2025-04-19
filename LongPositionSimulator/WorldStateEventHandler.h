#pragma once

struct CollissionPoint;
class SpaceObject;

class WorldStateEventHandler
{
public:
    ~WorldStateEventHandler() = default;

    virtual void onNewObject(SpaceObject obj) const;
    virtual void onDeleteObject(SpaceObject obj) const;
    virtual void onObjectChangedCourse(SpaceObject old, SpaceObject newObj) const;
    virtual void onUpdate(const SpaceObject& obj) const;


    virtual void onNewFutureCollission(CollissionPoint coll) const;
    virtual void onDeleteFutureCollission(CollissionPoint coll) const;
    virtual void onCollissionHappened(CollissionPoint coll) const;
};

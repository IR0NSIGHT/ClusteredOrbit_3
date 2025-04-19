#pragma once
#include "PlayerState.h"
#include "SpaceObject.h"

class WorldState;
struct CollissionPoint;

struct ObjectChanged
{
    SpaceObject oldObj;
    SpaceObject newObj;
};


class BasicPuppetMaster
{
public:
    BasicPuppetMaster() : currentTime(0), oldState(nullptr), newState(nullptr)
    {
    }

    ~BasicPuppetMaster()
    {
        // DESTROY
    }

private:
    //current global time
    double currentTime;
    double updateDelta;
    const WorldState* oldState;
    WorldState* newState;
    double clickedX;
    double clickedY;
    int playerFireWeapon = 0;

public:
    PlayerState playerState;
    std::vector<ObjectChanged> changedObjects;

public:
    void onPlayerInputThrustTowards(double x, double y)
    {
        clickedX = x;
        clickedY = y;
    }

    void onPlayerSelectObject(unsigned int targetId)
    {
        std::cout << "PLAYER TARGET " << targetId << std::endl;
        this->playerState.targetGlobalId = targetId;
    }

    void onPlayerSelectWeapon(Weapon weapon)
    {
        std::cout << "PLAYER WEAPON " << weapon << std::endl;
        this->playerState.selectedWeapon = weapon;
    }

    void onPlayerInputFireMissile()
    {
        playerFireWeapon++;
    }

    void setUp(const WorldState* oldState, WorldState* newState);
    void setTotalTime(double time);

    void onNewObject(SpaceObject obj) const;

    void onDeleteObject(SpaceObject obj) const;

    void onObjectChangedCourse(SpaceObject old, SpaceObject newObj) const;

    void onUpdate(const SpaceObject& obj);


    void onNewFutureCollission(CollissionPoint coll) const;

    void onDeleteFutureCollission(CollissionPoint coll) const;

    void onCollissionHappened(CollissionPoint coll) const;
};

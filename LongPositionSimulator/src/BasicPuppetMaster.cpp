#include "../include/BasicPuppetMaster.h"

#include <cassert>
#include <iostream>

#include "../include/Collission.h"
#include "../include/MagicMissile.h"
#include "../include/WorldState.h"
#include "RouterPlanner.h"

void BasicPuppetMaster::setUp(const WorldState* oldState, WorldState* newState)
{
    assert(oldState);
    assert(newState);
    this->oldState = oldState;
    this->newState = newState;
}

void BasicPuppetMaster::setTotalTime(double time)
{
    updateDelta = time - currentTime;
    currentTime = time;
    std::cout << "WORLDSTATE UPDATE AT " << time << std::endl;
}

void BasicPuppetMaster::onNewObject(SpaceObject obj) const
{
    std::cout << "New object added: " << obj << ", " << obj.lifetime << std::endl;
    if (obj.meta.type == ObjectType::PROJECTILE && obj.meta.faction == OpFor)
    {
        // bluship evasive maneuver
        auto blushipMaybe = oldState->getObject(BLUSHIP_ID, currentTime);
        if (blushipMaybe)
        {
            auto bluship = blushipMaybe.value().objectAt(currentTime);
            //find vector perpendicular to current thrust and projectile velocity
            auto evadeVector = vel3d::crossProduct(bluship.getCurrentPosObj().acceleration.normalized(),
                                                   obj.getCurrentPosObj().velocity.normalized());
            bluship.getCurrentPosObj().acceleration = evadeVector * 10; // upwards thrust
            this->newState->putObject(bluship.objectAt(-currentTime), currentTime);
        }
    }
}

void BasicPuppetMaster::onDeleteObject(SpaceObject obj) const
{
    //std::cout << "Object deleted: " << obj << std::endl;
}

void BasicPuppetMaster::onObjectChangedCourse(SpaceObject old, SpaceObject newObj) const
{
    if (!newObj.lifetime.existsAt(currentTime))
        return;
    std::cout << "Object " << old << " changed course" << std::endl;
}

bool usedManeuver = false;

void BasicPuppetMaster::onUpdate(const SpaceObject& obj)
{
    if (!obj.lifetime.existsAt(currentTime))
        return;
    if (obj.updater != nullptr)
        obj.updater->onUpdate(obj, oldState, newState, currentTime, updateDelta);

    SpaceObject objectNow(obj.objectAt(currentTime));

    if (obj.globalObjectId == BLUSHIP_ID) {
        auto maneuver = TimedManeuver{2.0, acc3d(1,2,3)};
        if (!usedManeuver) {
           auto updated = maneuver.applyManeuver(obj);
           newState->putObject(updated, maneuver.absTime);
           usedManeuver = true;
        }
    }

    if (obj.globalObjectId == REDSHIP_ID)
    {
        auto objNow = obj.objectAt(currentTime);

        // HANDLE THRUST TOWARDS INPUT
        if (clickedX != 0)
        {
            objectNow.getCurrentPosObj().acceleration = acc3d{
                clickedX, clickedY, 0
            }.normalized() * 20;
            this->clickedX = 0;
            std::cout << "UPDATE PLAYER SHIP thrust vector for worldstate " << oldState << std::endl;
            newState->putObject(objectNow.objectAt(-currentTime), currentTime);
        }

        // HANDLE FIRE MISSILE INPUT
        if (playerState.targetGlobalId != 0)
        {
            auto target = oldState->getObject(playerState.targetGlobalId, currentTime);
            if (target)
            {
                double fireTime = currentTime;
                for (int i = 0; i < playerFireWeapon; i++)
                {
                    if (fireTime >= currentTime + updateDelta)
                        break;
                    switch (playerState.selectedWeapon)
                    {
                    case Weapon::POINTDEFENSE:
                        {
                            break;
                        }
                    case Weapon::RAILGUN:
                        {
                            for (int j = 0; j < 2; j++) // fire a burst at target
                            {
                                auto bullet = MagicMissile::ShipFireAtTarget(
                                    obj, target.value(), lifeTime{fireTime, 10000}, 4000, 0);
                                if (bullet)
                                {
                                    newState->putObject(bullet.value(), currentTime);
                                    std::cout << "blufor" << objectNow << " fire railgun" << std::endl;
                                }
                                fireTime += 0.1;
                            }
                            break;
                        }
                    case Weapon::MISSILE:
                        {
                            auto missileSolution = MagicMissile::missileIntercept(
                                obj.objectAt(fireTime), target.value().objectAt(fireTime), 500);
                            if (missileSolution)
                            {
                                auto missile = missileSolution.value().obj1;
                                missile.globalObjectId = SpaceObject::nextId++;
                                newState->putObject(missile.objectAt(-fireTime), fireTime);
                                fireTime += 0.4;
                            }
                            break;
                        }
                    }
                }
            }
        }
        playerFireWeapon = 0;
    }
}

void BasicPuppetMaster::onNewFutureCollission(CollissionPoint coll) const
{
    std::cout << "New future collision detected at time: " << coll.time << std::endl;
}

void BasicPuppetMaster::onDeleteFutureCollission(CollissionPoint coll) const
{
    std::cout << "Future collision deleted at time: " << coll.time << std::endl;
}

void BasicPuppetMaster::onCollissionHappened(CollissionPoint coll) const
{
    if (coll.obj1.meta.type == ObjectType::PROJECTILE && coll.obj2.meta.type == ObjectType::PROJECTILE)
        return; //ignore bullets hitting eachother.

    std::cout << "Collision happened at time: " << coll.time << " between " << coll.obj1 << " and " << coll.obj2 <<
        "\n";
    std::cout << " distance at collission:" << coll.obj1.getCurrentPosObj().distanceToObjectAt(
        coll.time, coll.obj2.getCurrentPosObj()) << "\n";
    std::cout << " position of collission:\n" << coll.obj1 << ": " << coll.obj1.getCurrentPosObj().posAt(coll.time) << " vs "
        << coll.obj2 << ": " << coll.obj2.getCurrentPosObj().posAt(coll.time) << std::endl;
}

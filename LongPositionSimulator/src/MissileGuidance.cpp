#include "../include/MissileGuidance.h"
#include "../include/MagicMissile.h"
#include "../include/WorldState.h"

void MissileGuidance::onUpdate(const SpaceObject& obj, const WorldState* const currentState, WorldState* outState,
                               double currentTime, double updateDelta)
{
    auto missile = obj;

    auto target = currentState->getObject(targetGlobalId, currentTime);
    if (!target)
        return;

    //test if update is requried
    auto coll = Collission::nextCollissionFast(obj, target.value());
    if (coll)
        return; //missile hits, no update required.

    auto missileSolution = MagicMissile::missileIntercept(missile.objectAt(currentTime),
                                                          target.value().objectAt(currentTime), thrust);
    if (missileSolution)
    {
        auto updatedMissile = missileSolution.value().obj1.objectAt(-currentTime);
        missile.getCurrentPosObj() = updatedMissile.getCurrentPosObj();
        outState->putObject(missile, currentTime); //write back updated
        //std::cout << missile <<" adjust course with a=" << updatedMissile.getCurrentPosObj().acceleration.norm() << " target=" << target.value() << std::endl;
    }
}

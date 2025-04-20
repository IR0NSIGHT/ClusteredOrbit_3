#include "../include/PointDefenseAI.h"

#include "../include/MagicMissile.h"
#include "../include/MissileGuidance.h"
#include "../include/shipIds.h"
#include "../include/WorldState.h"

std::unique_ptr<Updateable> PointDefenseAI::clone() const
{
    return std::make_unique<PointDefenseAI>(this->meta);
}

void PointDefenseAI::onUpdate(const SpaceObject& thisShip, const WorldState* const currentState, WorldState* outState,
                              double currentTime, double updateDelta)
{
    // defend against missiles
    std::vector<SpaceObject> enemyMissiles;
    enemyMissiles.reserve(10);
    for (const auto& obj : currentState->objects)
    {
        if (!obj.lifetime.existsAt(currentTime))
            continue;
        if (obj.meta.type == ObjectType::MISSILE && obj.meta.faction != this->meta.faction)
        {
            // test if missile is targetting us.
            if (obj.updater != nullptr)
            {
                auto missileGuidance = static_cast<MissileGuidance*>(obj.updater.get());
                if (thisShip.globalObjectId != missileGuidance->targetGlobalId)
                {
                    continue;
                }
            }
            enemyMissiles.push_back(obj);
        }
    }

    // fire at each missile
    double fireTime = currentTime;
    double fireRate = 60. / 800;

    if (enemyMissiles.size() == 0)
        return;


    acc3d totalAcc{};
    for (auto& missile : enemyMissiles)
    {
        totalAcc = totalAcc + missile.posObj.acceleration;
    }
    totalAcc = totalAcc / enemyMissiles.size();
    //thrust away from missiles
    auto updatedSelf = SpaceObject(thisShip.objectAt(currentTime));
    updatedSelf.posObj.acceleration = totalAcc.normalized() * 10; // FIXME use ship thrust here.
    //outState->putObject(updatedSelf.objectAt(-currentTime));

    for (int i = 0; i < 20; i++)
    {
        for (auto& missile : enemyMissiles)
        {
            // only for duration till next update
            if (fireTime > currentTime + FIXED_UPDATE_DELTA)
                return;
            auto missileHitColl = Collission::nextCollissionFast(thisShip, missile);
            if (!missileHitColl) //ignore missiles that will miss
                continue;
            if (missile.posObj.distanceToObjectAt(currentTime, thisShip.posObj) > PD_TURRET_RANGE)
                //max radar range for defense
                continue;
            auto bullet = MagicMissile::ShipFireAtTarget(thisShip, missile,
                                                         lifeTime{fireTime += fireRate, missileHitColl->time - .25},
                                                         1000,
                                                         0.001);
            if (bullet)
            {
                bullet.value().posObj.radius = 1;
                bullet.value().globalObjectId = SpaceObject::nextId++;
                outState->putObject(bullet.value(), currentTime);
            }
        }
    }
}

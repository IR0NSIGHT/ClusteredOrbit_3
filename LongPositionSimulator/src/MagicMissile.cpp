#include "../include/MagicMissile.h"

#include <cassert>
#include <random>

#include "../include/Collission.h"
#include "../include/linearInterceptEquation.h"
#include "../include/MissileGuidance.h"
#include "../include/SpaceObject.h"


template <typename Numeric, typename Generator>
Numeric MagicMissile::random(Numeric from, Numeric to)
{
    thread_local Generator gen(std::random_device{}());

    using dist_type = std::conditional_t
    <
        std::is_integral_v<Numeric>
        , std::uniform_int_distribution<Numeric>
        , std::uniform_real_distribution<Numeric>
    >;

    thread_local dist_type dist;

    return dist(gen, typename dist_type::param_type{from, to});
}

bool isValidDouble(double value)
{
    return !std::isinf(value) && !std::isnan(value);
}

/**
 * \brief calculate a missile that can thrust with @missileAcc towards target.
 * \param gunShip 
 * \param target 
 * \param timeStart 
 * \param missileAcc 
 * \return 
 */
std::optional<CollissionPoint> MagicMissile::missileIntercept(const SpaceObject& gunShip, const SpaceObject& target,
                                                              double missileAcc)
{
    int xFactor = gunShip.posObj.position.x < target.posObj.position.x ? 1 : -1;
    int yFactor = gunShip.posObj.position.y < target.posObj.position.y ? 1 : -1;
    int zFactor = gunShip.posObj.position.z < target.posObj.position.z ? 1 : -1;

    auto xDist = polynomPower4{
        0, 0, 0.5 * (target.posObj.acceleration.x - xFactor * missileAcc),
        target.posObj.velocity.x - gunShip.posObj.velocity.x,
        target.posObj.position.x - gunShip.posObj.position.x
    };
    auto yDist = polynomPower4{
        0, 0, 0.5 * (target.posObj.acceleration.y - yFactor * missileAcc),
        target.posObj.velocity.y - gunShip.posObj.velocity.y,
        target.posObj.position.y - gunShip.posObj.position.y
    };
    auto zDist = polynomPower4{
        0, 0, 0.5 * (target.posObj.acceleration.z - zFactor * missileAcc),
        target.posObj.velocity.z - gunShip.posObj.velocity.z,
        target.posObj.position.z - gunShip.posObj.position.z
    };

    //find time points where on one axis + give missile thrust, the distance on this axis is zero 
    std::vector<double> criticalTs;
    criticalTs.reserve(6);
    auto xRoots = xDist.quadratricFindRoots();
    auto yRoots = yDist.quadratricFindRoots();
    auto zRoots = zDist.quadratricFindRoots();
    criticalTs.insert(criticalTs.end(), xRoots.begin(), xRoots.end());
    criticalTs.insert(criticalTs.end(), yRoots.begin(), yRoots.end());
    criticalTs.insert(criticalTs.end(), zRoots.begin(), zRoots.end());
    std::sort(criticalTs.begin(), criticalTs.end());

    if (target.posObj.acceleration.norm() > missileAcc)
        return std::nullopt;

    auto bestMissile = SpaceObject(positionable{});
    double bestTime = -1;
    if (criticalTs.size() == 0)
    {
        return std::nullopt;
    }
    double tLower = 0;
    double tHigher = 1000;
    for (int i = 0; i < 100; i++)
    //binary search with assumptions: required thrust for flighttime=0 is very large, for flighttime = 1000 is very low.
    {
        double t = (tLower + tHigher) / 2;
        SpaceObject missile = magicMissile(gunShip, target, 0, t);
        double diff = missile.posObj.acceleration.norm() - missileAcc;

        if (abs(diff) < .1)
        {
            return CollissionPoint(missile, target, t, bestMissile.posObj.posAt(bestTime));
        }
        if (diff > 0)
        {
            tLower = t;
        }
        if (diff < 0)
        {
            tHigher = t; //middle of interval
        }
    }

    return std::nullopt;
}

SpaceObject MagicMissile::magicMissile(const SpaceObject& gunShip, const SpaceObject& target, double timeStart,
                                       double flightTime)
{
    positionable gunStart = gunShip.posObj.objectAt(timeStart);
    positionable targetStart = target.posObj.objectAt(timeStart);

    pos3d missilePos = gunStart.posAt(0);
    vel3d missileVel = gunStart.velAt(0) + gunStart.velAt(0).normalized() * 100;
    pos3d targetPosFinal = targetStart.posAt(flightTime);
    //find missile acc
    acc3d missileAcc = ((targetPosFinal - missilePos - (missileVel * flightTime)) * 2) / (flightTime * flightTime);

    auto missile = positionable{missilePos, missileVel, missileAcc, 5};
    assert((missile.posAt(flightTime)-targetStart.posAt(flightTime)).norm() < missile.radius + targetStart.radius);
    assert((missile.posAt(0)-gunStart.posAt(0)).norm() <= 0.1);

    double delay = LinearIntercept::findFirstTimeComponentDistanceGreaterThan(missile, gunStart,
                                                                              missile.radius + gunStart.radius + 1);
    SpaceObject outMissile = SpaceObject(
        missile, lifeTime{delay, 100000}, ObjectMetaInfo{ObjectType::MISSILE, gunShip.meta.faction},
        SpaceObject::nextId++
    ).objectAt(-timeStart);

    outMissile.updater = std::make_unique<MissileGuidance>(target.globalObjectId,
                                                           outMissile.posObj.acceleration.norm());
    assert(outMissile.posObj.radius == 5);
    auto coll = Collission::nextCollissionFast(gunShip, outMissile);
    assert(!coll.has_value() || coll->time > gunShip.lifetime.start || coll->time > target.lifetime.start);
    // delay life start so missile is out of bbx of gunShip
    return outMissile;
}

std::optional<SpaceObject> MagicMissile::ShipFireAtTarget(const SpaceObject& gunShip, const SpaceObject& target,
                                                          lifeTime timeInterval, double bulletSpeed,
                                                          double precisionError)
{
    double timeShift = timeInterval.start;
    positionable gunInT = gunShip.posObj.objectAt(timeShift);
    positionable targetInT = target.posObj.objectAt(timeShift);

    std::vector<LinearIntercept> intercepts = LinearIntercept::findInterceptions(targetInT, gunInT, bulletSpeed);
    //cant shoot at target if it doesnt exist yet. only consider solutions after life start.
    //find solution for object interception shifted to t=timeStart
    for (auto solution : intercepts)
    {
        if (solution.timeOfIntercept > timeInterval.end - timeShift)
            continue; //do not shoot beyond time interval
        assert(
            solution.bullet.distanceToObjectAt( solution.timeOfIntercept, targetInT) <= solution.bullet.radius +
            targetInT.radius); //raw solution actually itnercepts target
        auto bullet = solution.bullet;
        bullet.radius = 1;
        double timeToLeaveBbx = LinearIntercept::findFirstTimeComponentDistanceGreaterThan(gunInT, bullet,
            (gunInT.radius + bullet.radius) + 1);
        SpaceObject out = SpaceObject(
            bullet, lifeTime{timeToLeaveBbx, 100000}, ObjectMetaInfo{ObjectType::PROJECTILE, gunShip.meta.faction},
            SpaceObject::nextId++
        ).objectAt(-timeShift);

        assert(
            out.posObj.distanceToObjectAt(timeShift+ solution.timeOfIntercept, target.posObj) <= target.
            posObj.radius + out.posObj.radius); //normalized bullet hits target
        // DEBUG
        auto coll = Collission::nextCollissionFast(out, target);
        assert(coll.has_value());

        if (precisionError != 0)
        {
            vel3d vel = out.posObj.velocity;
            out.posObj.velocity = vel3d(vel.x * random(-precisionError + 1, precisionError + 1),
                                              vel.y * random(-precisionError + 1, precisionError + 1),
                                              vel.z * random(-precisionError + 1, precisionError + 1));
        }
        return out;
    }
    return std::nullopt;
}

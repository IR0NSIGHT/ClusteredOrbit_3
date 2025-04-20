#pragma once
#include <optional>
#include <random>

struct CollissionPoint;
struct lifeTime;
class SpaceObject;

class MagicMissile
{
public:
    template <typename Numeric, typename Generator = std::mt19937>
    static Numeric random(Numeric from, Numeric to);
    static SpaceObject magicMissile(const SpaceObject& gunShip, const SpaceObject& target, double timeStart,
                                    double flightTime);

    static std::optional<CollissionPoint> missileIntercept(const SpaceObject& gunShip, const SpaceObject& target,
                                                           double missileAcc);

    /**
     * \brief make gunShip fire a single bullet at target at time=timeStart. bullet will start existing shortly after leaving the gunships BBX
     * \param gunShip bullet inherits ships faction.
     * \param target 
     * \param timeStart 
     * \param bulletSpeed desired speed of bullet. outSpeed <= bulletSpeed is guaranteed.
     * \return bullet object normalized to t=0
     */
    static std::optional<SpaceObject> ShipFireAtTarget(const SpaceObject& gunShip, const SpaceObject& target,
                                                       lifeTime timeInterval,
                                                       double bulletSpeed, double precisionError = 0.00825);
};

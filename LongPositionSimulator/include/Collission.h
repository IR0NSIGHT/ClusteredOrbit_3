#pragma once
#include <cassert>
#include <algorithm>
#include "polynomPower4.h"
#include "shipIds.h"
#include "SpaceObject.h"

struct CollissionPoint
{
    explicit CollissionPoint(SpaceObject o1, SpaceObject o2, double time, pos3d position): obj1(o1), obj2(o2),
        time(time), positions(position)
    {
    }

    SpaceObject obj1;
    SpaceObject obj2;
    double time;
    pos3d positions;

    pos3d offsetAtCollission()
    {
        return obj1.posObj.posAt(time) - obj2.posObj.posAt(time);
    }
};

class Collission
{
public:
    static polynomPower4_3d distanceOfObjects(const positionable& o1, const positionable& o2)
    {
        polynomPower4 xDist = distancBetweenObjectsByTime(o1.position.x, o2.position.x, o1.velocity.x,
                                                  o2.velocity.x, o1.acceleration.x,
                                                  o2.acceleration.x);
        polynomPower4 yDist = distancBetweenObjectsByTime(o1.position.y, o2.position.y, o1.velocity.y,
                                                          o2.velocity.y, o1.acceleration.y,
                                                          o2.acceleration.y);
        polynomPower4 zDist = distancBetweenObjectsByTime(o1.position.z, o2.position.z, o1.velocity.z,
                                                          o2.velocity.z, o1.acceleration.z,
                                                          o2.acceleration.z);
        return polynomPower4_3d{ xDist, yDist, zDist};
    }
    
    static polynomPower4 distanceSquaredBetweenObjectsByTime(double p1, double p2, double v1, double v2, double a1,
                                                             double a2)
    {
        double t4 = 0.25 * a1 * a1 + 0.25 * a2 * a2 - 0.5 * a1 * a2;
        double t3 = a1 * v1 - a2 * v1 - a1 * v2 + a2 * v2;
        double t2 = a1 * p1 - a2 * p1 - a1 * p2 + a2 * p2 + v1 * v1 + v2 * v2 - 2 * v1 * v2;
        double t = 2 * p1 * v1 - 2 * p2 * v1 - 2 * p1 * v2 + 2 * p2 * v2;
        double d = p1 * p1 + p2 * p2 - 2 * p1 * p2;
        return polynomPower4{t4, t3, t2, t, d};
    }

    static polynomPower4 distancBetweenObjectsByTime(double p1, double p2, double v1, double v2, double a1,
                                                     double a2)
    {
        polynomPower4 pos1{0, 0, 0.5 * a1, v1, p1};
        polynomPower4 pos2{0, 0, 0.5 * a2, v2, p2};
        return pos2 - pos1;
    }

    static polynomPower4 distanceSquaredBetweenObjectsByTime(const positionable& o1, const positionable& o2)
    {
        polynomPower4 xDistSq = distanceSquaredBetweenObjectsByTime(o1.position.x, o2.position.x, o1.velocity.x,
                                                                    o2.velocity.x, o1.acceleration.x,
                                                                    o2.acceleration.x);
        polynomPower4 yDistSq = distanceSquaredBetweenObjectsByTime(o1.position.y, o2.position.y, o1.velocity.y,
                                                                    o2.velocity.y, o1.acceleration.y,
                                                                    o2.acceleration.y);
        polynomPower4 zDistSq = distanceSquaredBetweenObjectsByTime(o1.position.z, o2.position.z, o1.velocity.z,
                                                                    o2.velocity.z, o1.acceleration.z,
                                                                    o2.acceleration.z);
        polynomPower4 distSqAll = xDistSq + yDistSq + zDistSq;
        return distSqAll;
    }

    static std::vector<double> collissionTimes(const positionable& o1, const positionable& o2)
    {
        polynomPower4 distanceSquared = distanceSquaredBetweenObjectsByTime(o1, o2);
        distanceSquared.e -= (o1.radius + o2.radius) * (o1.radius + o2.radius);
        // d^2(t) == radiusDist^2 <==> collission occurs)
        return distanceSquared.findRoots();
    }

    static std::optional<CollissionPoint> nextCollissionFast(const SpaceObject& o1, const SpaceObject& o2);

    static double nextExtremePointDistanceBetween(positionable o1, positionable o2, double minTime = 0)
    {
        polynomPower4 cubeEdgeLength = distanceSquaredBetweenObjectsByTime(o1, o2);
        if (cubeEdgeLength.c < 0)
        {
            //parabola open to bottom, extreme point is a maximum
        }
        polynomPower4 derivative = cubeEdgeLength.derive();
        auto t_of_min_dist = derivative.findRoots();
        for (auto t : t_of_min_dist)
            if (t >= minTime)
                return t;

        return -1;
    }
};

inline std::optional<CollissionPoint> Collission::nextCollissionFast(const SpaceObject& objA, const SpaceObject& objB)
{
    const auto o1 = objA.posObj;
    const auto o2 = objB.posObj;
    polynomPower4 xDist = distancBetweenObjectsByTime(o1.position.x, o2.position.x, o1.velocity.x,
                                                      o2.velocity.x, o1.acceleration.x,
                                                      o2.acceleration.x);
    polynomPower4 yDist = distancBetweenObjectsByTime(o1.position.y, o2.position.y, o1.velocity.y,
                                                      o2.velocity.y, o1.acceleration.y,
                                                      o2.acceleration.y);
    polynomPower4 zDist = distancBetweenObjectsByTime(o1.position.z, o2.position.z, o1.velocity.z,
                                                      o2.velocity.z, o1.acceleration.z,
                                                      o2.acceleration.z);
    auto distanceSquared3d = xDist.square() + yDist.square() + zDist.square();
    //special case: constant distance


    int radii = o1.radius + o2.radius;
    if (distanceSquared3d.solveFor(0) <= (radii * radii))
    {
        double t = 0;
        //calculate where the intersection of bounding spheres happens.
        pos3d o1P = o1.posAt(t);
        pos3d o2P = o2.posAt(t);
        double o1Factor = (static_cast<double>(o1.radius) / radii);
        double o2Factor = (static_cast<double>(o2.radius) / radii);
        pos3d interpolatedPos = o1P * o2Factor + o2P * o1Factor;
        assert((o1P-o2P).norm() <= o1.radius+o2.radius + 0.001);
        return CollissionPoint{objA, objB, t, interpolatedPos};
    }
    if (distanceSquared3d.power() == 0)
        return std::nullopt;
    xDist.e -= radii;
    yDist.e -= radii;
    zDist.e -= radii;
    distanceSquared3d.e -= radii * radii;

    std::vector<double> criticalTs;
    criticalTs.reserve(6);
    auto xRoots = xDist.quadratricFindRoots();
    if (xRoots.empty() && xDist.solveFor(0) > 0)
        return std::nullopt;
    auto yRoots = yDist.quadratricFindRoots();
    if (yRoots.empty() && yDist.solveFor(0) > 0)
        return std::nullopt;
    auto zRoots = zDist.quadratricFindRoots();
    if (zRoots.empty() && zDist.solveFor(0) > 0)
        return std::nullopt;
    criticalTs.insert(criticalTs.end(), xRoots.begin(), xRoots.end());
    criticalTs.insert(criticalTs.end(), yRoots.begin(), yRoots.end());
    criticalTs.insert(criticalTs.end(), zRoots.begin(), zRoots.end());
    std::sort(criticalTs.begin(), criticalTs.end());

    //  std::cout << "d_x(x) =" << xDist << std::endl;
    //  std::cout << "d_y(x) =" << yDist << std::endl;
    //  std::cout << "d_z(x) =" << zDist << std::endl;
    xDist.e += radii; //xDist is now distance on x axis between objs.
    yDist.e += radii;
    zDist.e += radii;
    bool aabbxCollission = false;
    for (double t : criticalTs)
    {
        if (t >= 0 &&
            abs(xDist.solveFor(t)) <= radii + 0.01 &&
            abs(yDist.solveFor(t)) <= radii + 0.01 &&
            abs(zDist.solveFor(t)) <= radii + 0.01)
        {
            aabbxCollission = true;
            break;
        }
    }
    if (aabbxCollission)
    {
        for (double t : distanceSquared3d.findRoots())
        {
            if (t >= 0) //dont condiser collissions past 24h
            {
                if (t > MAX_TIME_COLLISSION)
                    continue;
                //calculate where the intersection of bounding spheres happens.
                pos3d o1P = o1.posAt(t);
                pos3d o2P = o2.posAt(t);
                double o1Factor = (static_cast<double>(o1.radius) / radii);
                double o2Factor = (static_cast<double>(o2.radius) / radii);
                pos3d interpolatedPos = o1P * o2Factor + o2P * o1Factor;
                assert((o1P-o2P).norm() <= o1.radius+o2.radius + 0.001);
                return CollissionPoint{objA, objB, t, interpolatedPos};
            }
        }
    }
    return std::nullopt;
}

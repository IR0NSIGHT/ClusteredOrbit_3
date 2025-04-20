#pragma once
#include <vector>
#include "vel3d.h"
#include "polynomPower4.h"
#include "positionable.h"
#include <cassert>

struct LinearIntercept
{
    positionable bullet{};
    double timeOfIntercept;

public:
    // returns l(t) = v(t)^2 * t^2, partial step towards calculating the velocity vector to hit an accelerated target with specified bullet speed
    static polynomPower4 lT(double s, double v, double a, double p)
    {
        a = a / 2;
        return polynomPower4{
            a * a,
            3 * a * v,
            -2 * a * p + 2 * a * s + v * v,
            (-2 * p * v + 2 * s * v),
            (p * p - 2 * p * s + s * s)
        };
    }

    static void testIntercept()
    {
        auto target = positionable{pos3d{0, 20, 30}, vel3d{0, 0, 0}, acc3d{1, 0, 0}, 1}; //flies 12.5m in 5 seconds on x
        double bulletSpeed = 20;
        pos3d gunPosition{12.5 - 100, 20, 30};

        //fix impact time t = 5s
        //fix impact pos = pos3d{12.5,20,30}
        // gun velocity is 20m/s, t=5 => 100m flight distance from impact point {12.5,20,30}
        double t = 5;
        auto bullet = positionable{gunPosition, vel3d{bulletSpeed, 0, 0}, acc3d{0, 0, 0}, 1};
        pos3d targetPosAt5 = target.posAt(5);
        pos3d bulletPosAt5 = bullet.posAt(t);
        pos3d impactPos{12.5, 20, 30};
        assert(impactPos == bulletPosAt5); //test designed setup if it actually is a correct setup
        assert(targetPosAt5 == impactPos);

        auto gun = positionable{gunPosition, vel3d{0, 0, 0}, acc3d{0, 0, 0}, 1};
        std::vector<LinearIntercept> intercepts = findInterceptions(target, gun, bulletSpeed);
        for (auto intercept : intercepts)
        {
            assert(intercept.timeOfIntercept >= 0);
            assert(round(intercept.bullet.velocity.norm()) == bulletSpeed);
            pos3d bulletPos = intercept.bullet.posAt(intercept.timeOfIntercept);
            pos3d targetPos = target.posAt(intercept.timeOfIntercept);
            assert(bulletPos == targetPos);
        }
    }


    static std::vector<double> linearInterceptEquation(pos3d position, const vel3d& gunShipVel,
                                                       const positionable& target, double bulletSpeed);

    static double findFirstTimeComponentDistanceGreaterThan(positionable o1, positionable o2, double distance);


    static std::vector<LinearIntercept> findInterceptions(positionable target, positionable gun, double bulletSpeed,
                                                          double minTime = 0)
    {
        //find time stamps when firing the bullet+hitting target would be possible 
        auto fireTimes = linearInterceptEquation(gun.position, gun.velocity, target, bulletSpeed);
        std::vector<LinearIntercept> interceptions;
        interceptions.reserve(fireTimes.size());

        for (double time : fireTimes)
        {
            // at time t, there exists a velocity vector v with ||v|| == gunspeed that would hit the target when fired at t=0
            if (time < minTime)
                continue;
            //find velocity vector
            auto targetPos = target.posAt(time);
            auto dir = vel3d{targetPos - gun.position};

            //floating point imprecision causes quite bad deviation from wanted solution, so we cheat and adjust the fire solution: change dir vector length to be compliant with gunspeed
            double ratio = (targetPos - gun.position).norm() / dir.norm();
            // scale the route to be exact gunPos to targetPos
            dir = dir * ratio;
            auto gunVelocity = dir / time;
            auto bullet = positionable{targetPos - dir, gunVelocity, {0, 0, 0}, 1};
            assert((bullet.posAt(time) - target.posAt(time)).norm() <= bullet.radius +target.radius);
            //assert(gunVelocity.norm() <= bulletSpeed);	//travelled path is compliant with bullet speed
            interceptions.push_back(LinearIntercept{bullet, time});
        }
        return interceptions;
    }
};

#include "linearInterceptEquation.h"

#include <iostream>

#include "Collission.h"


std::vector<double> LinearIntercept::linearInterceptEquation(const pos3d position, const vel3d& gunShipVel,
                                                             const positionable& target, double bulletSpeed)
{
    // l(t) = v(t)^2 * t^2
    auto ltX = lT(target.position.x, target.velocity.x - gunShipVel.x, target.acceleration.x, position.x);
    auto ltY = lT(target.position.y, target.velocity.y - gunShipVel.y, target.acceleration.y, position.y);
    auto ltZ = lT(target.position.z, target.velocity.z - gunShipVel.z, target.acceleration.z, position.z);
    auto mSq = polynomPower4{0, 0, bulletSpeed * bulletSpeed, 0, 0};
    // implements: 0 == lx(t)+ly(t)+lz(t)-m^2*t^2 <==> m == sqrt(vx^2+vy^2+vz^2)
    auto equation = ltX + ltY + ltZ - mSq;
    std::vector<double> roots = equation.findRoots();
    return roots;
}

double LinearIntercept::findFirstTimeComponentDistanceGreaterThan(positionable o1, positionable o2, double distance)
{
    auto distance3d = Collission::distanceOfObjects(o1, o2);
    distance3d.x.e -= distance;
    distance3d.y.e -= distance;
    distance3d.z.e -= distance;
    std::vector<polynomPower4> equations(3);
    equations[0] = distance3d.x;
    equations[1] = distance3d.y;
    equations[2] = distance3d.z;
    for (auto eq : equations)
    {
        for (auto t : eq.quadratricFindRoots())
        {
            if (t >= 0 && o1.distanceToObjectAt(t, o2) >= distance)
                return t;
        }
    }
    return -1;
}

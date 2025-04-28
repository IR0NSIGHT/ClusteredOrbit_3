#include <iostream>
#include <vector>
#include <cassert>
#include "WorldState.h"
#include <cmath>
#include <functional>
#include <complex>

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>

#include "polynomPower4.h"
#include "linearInterceptEquation.h"
#include "Canvas.h"

int main()
{
    //LinearIntercept::testIntercept();
    Canvas c;

    {
        auto simpleQuadraticEq = polynomPower4{0,0,2,3,4};
        auto squared = simpleQuadraticEq.square();
        auto expected = polynomPower4{4,12,25,24,16};
        assert(squared == expected );
    }
    {
        // two accelerated objects DONT meet
        pos3d o2Pos{10, 20, 30};
        pos3d o1Pos{10, 22.1, 30};
        auto o1 = positionable{pos3d{10, 22.1, 30}, vel3d{5, 0, 0}, acc3d{1, 0, 0}, 1}.objectAt(-4);
        auto o2 = positionable{pos3d{10, 20, 30}, vel3d{-2, 0, 0}, acc3d{3, 0, 0}, 1}.objectAt(-4);
        assert(o1.distanceToObjectAt(4,o2) <= 2.1001);
        assert(o1.posAt(4) == o1Pos);
        assert(o2.posAt(4) == o2Pos);
        auto coll = Collission::nextCollissionFast(SpaceObject(o1), SpaceObject(o2));
        assert(!coll); //objects do not meet
    }

    {
        //super simple collission on x axis
        auto o1 = positionable{{10, 20, 30}, vel3d{}, acc3d{}, 1};
        auto o2 = positionable{{10 + 10, 20, 30}, vel3d{-2, 0, 0}, acc3d{}, 1};
        auto coll = Collission::nextCollissionFast(SpaceObject(o1), SpaceObject(o2));
        assert(abs(coll.value().time - 4) <= 0.0001);
    }
    
    {
        // regression test of bullet fired at target but collission said they dont intercept
        positionable bullet = {
            pos3d{1572.1022201498351, -1682.8319358818801, -5.0799142037856395},
            vel3d{-263.40888059934065, 445.07774352752261, 24.319656815142558},
            acc3d{0, 0, 0}, 1
        };
        positionable target = {pos3d{-2062.5, 3400, 25}, vel3d{75, 60, 10}, acc3d{-5, -8, 2}, 25};
        double interceptTime = 11.762352565256824;
        double distAtIntercept = bullet.distanceToObjectAt(interceptTime, target);
        assert(distAtIntercept < 0.1);

        auto coll = Collission::nextCollissionFast(SpaceObject(bullet), SpaceObject(target));
        assert(coll.has_value());
    } 

    
    {
        // two accelerated objects meet
        pos3d collPos{10, 20, 30};
        auto o1 = positionable{collPos, vel3d{5, 6, 7}, acc3d{1, 2, 3}, 1}.objectAt(-4);
        auto o2 = positionable{collPos, vel3d{-2, -3, 4}, acc3d{3, 2, 1}, 1}.objectAt(-4);
        assert(o1.posAt(4) == collPos);
        assert(o2.posAt(4) == collPos);
        auto coll = Collission::nextCollissionFast(SpaceObject(o1), SpaceObject(o2));
        assert(coll.value().time-3.83121 <= 0.001);
        //collission time is a bit earlier because of the boudning spheres surrouding the objects.
        assert((coll.value().positions - (o1.posAt(3.83121)+o2.posAt(3.83121))/2.).norm() <= 0.001);
    }

    {
        // simple 0 = 0 => always true
        auto roots = polynomPower4{0, 0, 0,0,0}.quadratricFindRoots();
        assert(roots.size() == 1);
        assert(roots[0] == 0);
    }
    
    {
        // simple x^2-4 = 0
        auto roots = polynomPower4{0, 0, 1, 0, -4}.quadratricFindRoots();
        assert(roots.size() == 2);
        assert(roots[0] == -2);
        assert(roots[1] == 2);
    }

    {
        auto o1 = positionable{{10, 20, 30}, vel3d{}, acc3d{}, 1};
        auto coll = Collission::nextCollissionFast(SpaceObject(o1), SpaceObject(o1));
        assert(coll.value().time == 0);
    }
    
    std::cout << "Hello World!\n";
    c.display();
}

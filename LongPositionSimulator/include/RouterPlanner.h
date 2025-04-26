//
// Created by klipper on 4/22/25.
//

#ifndef LONGPOSITIONSIMULATOR_ROUTERPLANNER_H
#define LONGPOSITIONSIMULATOR_ROUTERPLANNER_H

#include "SpaceObject.h"

struct TimedManeuver {
    //perform maneuver in x seconds
    double absTime;
    vel3d newAcceleration;

    positionable applyManeuver(const positionable& object) {
        auto out = object.objectAt(absTime);
        out.acceleration = newAcceleration;
        return out.objectAt(-absTime);
    }

    SpaceObject applyManeuver(const SpaceObject& object) {
        auto out = SpaceObject(object);
        out.getCurrentPosObj() = applyManeuver(out.getCurrentPosObj());
        out.lifetime.start = absTime;
        return out;
    }
};

#endif //LONGPOSITIONSIMULATOR_ROUTERPLANNER_H

//
// Created by klipper on 4/28/25.
//

#include <gtest/gtest.h>
#include "positionable.h"

TEST(Positionable, Basic) {
    auto obj = positionable{ pos3d(1,2,3), vel3d(4,5,6), acc3d(7,8,9), 5};
    EXPECT_EQ(obj.position, pos3d(1,2,3));
    EXPECT_EQ(obj.velocity, vel3d(4,5,6));
    EXPECT_EQ(obj.acceleration, acc3d(7,8,9));
    EXPECT_EQ(obj.radius, 5);
}

TEST(Positionable, posAndVelByTime) {
    auto obj = positionable{ pos3d(1,2,3), vel3d(4,5,6), acc3d(7,8,9),  5};
    EXPECT_EQ(obj.posAt(0), pos3d(1,2,3));
    EXPECT_EQ(obj.velAt(0), vel3d(4,5,6));
    //v = v0 + a*t
    EXPECT_EQ(obj.velAt(2), vel3d(4,5,6) + acc3d(7,8,9) * 2 );

    //s = 0.5 * a*t*t + v*t + s
    EXPECT_EQ(obj.posAt(2), pos3d(1,2,3) + vel3d(4,5,6) * 2 + acc3d(7,8,9) * 2 * 2 * 0.5);
}

TEST(Positionable, objectByTime) {
    auto obj = positionable{ pos3d(1,2,3), vel3d(4,5,6), acc3d(7,8,9),  5};
    auto objT = obj.objectAt(2);

    EXPECT_EQ(objT.position, obj.posAt(2));
    EXPECT_EQ(objT.velocity, obj.velAt(2));
    EXPECT_EQ(objT.acceleration, obj.acceleration);
    objT.radius = obj.radius;
}

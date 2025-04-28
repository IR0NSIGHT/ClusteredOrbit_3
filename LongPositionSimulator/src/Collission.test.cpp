//
// Created by klipper on 4/28/25.
//
#include <gtest/gtest.h>
#include "positionable.h"
#include "Collission.h"
#include "SpaceObject.h"

TEST(Collission, linear_movement_on_x) {
    //super simple collission on x axis
    auto o1 = positionable{{10, 20, 30}, vel3d{}, acc3d{}, 1};
    auto o2 = positionable{{10 + 10, 20, 30}, vel3d{-2, 0, 0}, acc3d{}, 1};
    auto coll = Collission::nextCollissionFast(SpaceObject(o1), SpaceObject(o2));
    EXPECT_DOUBLE_EQ(coll.value().time, 4);
}

TEST(Collission, acceleradted_movement_3d) {
    // two accelerated objects meet
    pos3d collPos{10, 20, 30};
    auto o1 = positionable{collPos, vel3d{5, 6, 7}, acc3d{1, 2, 3}, 1}.objectAt(-4);
    auto o2 = positionable{collPos, vel3d{-2, -3, 4}, acc3d{3, 2, 1}, 1}.objectAt(-4);

    auto coll = Collission::nextCollissionFast(SpaceObject(o1), SpaceObject(o2));
    double collTimeExpect = 3.8312144701399857;
    EXPECT_DOUBLE_EQ(coll.value().time,collTimeExpect);
    auto collPosExpect = (o1.posAt(collTimeExpect)+o2.posAt(collTimeExpect))/2.;
    EXPECT_DOUBLE_EQ(coll.value().positions.x, collPosExpect.x);
    EXPECT_DOUBLE_EQ(coll.value().positions.y, collPosExpect.y);
    EXPECT_DOUBLE_EQ(coll.value().positions.z, collPosExpect.z);
}

TEST(Collission, bullet_collided_regtest) {
    // regression test of bullet fired at target but collission said they dont intercept
    positionable bullet = {
            pos3d{1572.1022201498351, -1682.8319358818801, -5.0799142037856395},
            vel3d{-263.40888059934065, 445.07774352752261, 24.319656815142558},
            acc3d{0, 0, 0}, 1
    };
    positionable target = {pos3d{-2062.5, 3400, 25}, vel3d{75, 60, 10}, acc3d{-5, -8, 2}, 25};
    double interceptTime = 11.762352565256824;
    double distAtIntercept = bullet.distanceToObjectAt(interceptTime, target);
    EXPECT_TRUE(distAtIntercept < 0.1);

    auto coll = Collission::nextCollissionFast(SpaceObject(bullet), SpaceObject(target));
    EXPECT_TRUE(coll.has_value());
    EXPECT_TRUE(abs(coll->time - interceptTime) < 0.1);
    EXPECT_TRUE(bullet.collidesWithAt(target, coll->time));
}

TEST(Collission, object_collides_with_self) {
    auto o1 = positionable{{10, 20, 30}, vel3d{}, acc3d{}, 1};
    auto coll = Collission::nextCollissionFast(SpaceObject(o1), SpaceObject(o1));
    EXPECT_EQ(coll.value().time, 0);
}

TEST(Collission, Basic) {
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

    }
}
//
// Created by klipper on 4/28/25.
//

#include <gtest/gtest.h>
#include "polynomPower4.h"

TEST(Polynomial, TestCase1) {
    {
        // simple 0 = 0 => always true
        auto roots = polynomPower4{0, 0, 0,0,0}.quadratricFindRoots();
        EXPECT_EQ(roots.size(), 1);
        EXPECT_EQ(roots[0], 0);
    }

    {
        // simple x^2-4 = 0
        auto roots = polynomPower4{0, 0, 1, 0, -4}.quadratricFindRoots();
        EXPECT_EQ(roots.size() ,2);
        EXPECT_EQ(roots[0] ,-2);
        EXPECT_EQ(roots[1] ,2);
    }

    {
        auto simpleQuadraticEq = polynomPower4{0,0,2,3,4};
        auto squared = simpleQuadraticEq.square();
        auto expected = polynomPower4{4,12,25,24,16};
        EXPECT_EQ(squared ,expected );
    }
}

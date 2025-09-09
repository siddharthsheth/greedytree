#include <gtest/gtest.h>
#include "../include/greedy.hpp"
#include <vector>
#include <cmath>
#include <limits>

TEST(GreedyGonzalezTest, Trivial) {
    using MyPoint = Point<1, L1Metric>;

    // Empty input
    std::vector<MyPoint> empty_pts;
    std::vector<const MyPoint*> gp, pred;
    EXPECT_NO_THROW(gonzalez(empty_pts, gp, pred));
    EXPECT_TRUE(gp.empty());
    EXPECT_TRUE(pred.empty());

    // Single point
    std::vector<MyPoint> single_pt{MyPoint{42.0}};
    gp.clear(); pred.clear();
    gonzalez(single_pt, gp, pred);
    ASSERT_EQ(gp.size(), 1);
    ASSERT_EQ(pred.size(), 1);
    EXPECT_EQ(gp[0], &single_pt[0]);
    EXPECT_EQ(pred[0], &single_pt[0]);

    // Two points
    std::vector<MyPoint> two_pts{MyPoint{-1.0}, MyPoint{3.0}};
    gp.clear(); pred.clear();
    gonzalez(two_pts, gp, pred);
    ASSERT_EQ(gp.size(), 2);
    std::vector<double> found_coords{gp[0]->coords[0], gp[1]->coords[0]};
    std::vector<double> expected_coords{-1.0, 3.0};
    std::sort(found_coords.begin(), found_coords.end());
    std::sort(expected_coords.begin(), expected_coords.end());
    EXPECT_EQ(found_coords, expected_coords);
}


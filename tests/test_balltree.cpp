#include <gtest/gtest.h>
#include <memory>
#include "../include/balltree.hpp"

TEST(BallTreeTest, LeafInitialization) {
    using MyPoint = Point<3, L2Metric>;

    MyPoint pt(std::initializer_list<double>{1.0, 2.0, 3.0});
    BallTree<3, L2Metric> tree(&pt);

    EXPECT_TRUE(tree.isleaf());
    EXPECT_EQ(tree._size, 1);
    EXPECT_DOUBLE_EQ(tree.radius, 0.0);
}

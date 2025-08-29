#include <gtest/gtest.h>
#include "../include/balltree.hpp"

TEST(BallTreeTest, LeafInitialization) {
    using MyPoint = Point<3, L2Metric>;
    auto pt_ptr = std::make_shared<MyPoint>(std::initializer_list<double>{1.0, 2.0, 3.0});
    // MyPoint pt(std::initializer_list<double>{1.0, 2.0, 3.0});
    BallTree<3, L2Metric> tree(pt_ptr);

    // std::vector<std::unique_ptr<const MyPoint>> inp, gp, pred;
    // inp.push_back(std::move(pt));
    // clarkson(inp, gp, pred);

    EXPECT_TRUE(tree.isleaf());
    EXPECT_EQ(tree._size, 1);
    EXPECT_DOUBLE_EQ(tree.radius, 0.0);
}

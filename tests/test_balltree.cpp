#include <gtest/gtest.h>
#include <memory>
#include "../include/balltree.hpp"

TEST(BallTreeTest, LeafInitialization) {
    using MyPoint = std::array<double, 3>;
    L2Metric metric;

    MyPoint pt({1.0, 2.0, 3.0});
    const MyPoint *ptr = &pt;
    BallTree<3, L2Metric> tree(ptr, metric);

    EXPECT_TRUE(tree.isleaf());
    EXPECT_EQ(tree.size, 1);
    EXPECT_DOUBLE_EQ(tree.radius, 0.0);
}

// TEST(BallTreeTest, HeapTraversal) {
//     using Pt = Point<1, L1Metric>;

//     Pt pt({1.0, 2.0, 3.0});
//     BallTree<3, L2Metric> tree(&pt);

//     pts.push_back(MyPoint{double(i)});

//     EXPECT_TRUE(tree.isleaf());
//     EXPECT_EQ(tree._size, 1);
//     EXPECT_DOUBLE_EQ(tree.radius, 0.0);
// }

TEST(BallTreeTest, NearestNeighbor) {
    using PlanarPoint = std::array<double, 2>;
    using PlanarBallTreePtr = std::unique_ptr<BallTree<2, L1Metric>>;
    L1Metric metric;

    vector<PlanarPoint> pts;
    pts.push_back(PlanarPoint({0, 0}));
    pts.push_back(PlanarPoint({1, 2}));
    pts.push_back(PlanarPoint({5, 6}));
    pts.push_back(PlanarPoint({15, 0}));
    pts.push_back(PlanarPoint({8, 5}));

    PlanarBallTreePtr tree = greedy_tree(pts, metric);
    
    PlanarPoint query({15,7});
    const PlanarPoint* nn = tree->nearest(&query);

    EXPECT_EQ(*nn, PlanarPoint({15, 0}));
}

TEST(BallTreeTest, FarthestNeighbor) {
    using PlanarPoint = std::array<double, 2>;
    using PlanarBallTreePtr = std::unique_ptr<BallTree<2, L1Metric>>;
    L1Metric metric;

    vector<PlanarPoint> pts;
    pts.push_back(PlanarPoint({0, 0}));
    pts.push_back(PlanarPoint({1, 2}));
    pts.push_back(PlanarPoint({5, 6}));
    pts.push_back(PlanarPoint({15, 0}));
    pts.push_back(PlanarPoint({8, 5}));

    PlanarBallTreePtr tree = greedy_tree(pts, metric);
    
    PlanarPoint query({15,7});
    const PlanarPoint* fn = tree->farthest(&query);

    EXPECT_EQ(fn, &pts[0]);
}
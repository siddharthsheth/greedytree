// Type-parameterized tests for greedy clustering algorithms (Gonzalez and Clarkson)
#include <gtest/gtest.h>
#include "../include/greedy.hpp"
#include <vector>
#include <cmath>
#include <limits>

// struct GonzalezAlgo {
//     template <std::size_t d, typename Metric>
//     void operator()(std::vector<Point<d, Metric>>& pts,
//                     std::vector<const Point<d, Metric>*>& pred) const {
//         gonzalez(pts, pred);
//     }
// };
// struct ClarksonAlgo {
//     template <std::size_t d, typename Metric>
//     void operator()(std::vector<Point<d, Metric>>& pts,
//                     std::vector<const Point<d, Metric>*>& pred) const {
//         clarkson(pts, pred);
//     }
// };

struct GonzalezAlgo {
    template <std::size_t d, typename Metric>
    void operator()(std::vector<Point<d, Metric>>& pts,
                    std::vector<size_t>& pred) const {
        gonzalez(pts, pred);
    }
};
struct ClarksonAlgo {
    template <std::size_t d, typename Metric>
    void operator()(std::vector<Point<d, Metric>>& pts,
                    std::vector<size_t>& pred) const {
        clarkson(pts, pred);
    }
};

// Test fixture template
template <typename Algo>
class GreedyTest : public ::testing::Test {
public:
    using MyPoint = Point<1, L1Metric>;
    Algo algo;
};

TYPED_TEST_SUITE_P(GreedyTest);

TYPED_TEST_P(GreedyTest, Empty) {
    using MyPoint = typename TestFixture::MyPoint;
    TypeParam algo;
    std::vector<MyPoint> empty_pts;
    std::vector<size_t> pred;
    EXPECT_NO_THROW(algo(empty_pts, pred));
    EXPECT_TRUE(pred.empty());
}

TYPED_TEST_P(GreedyTest, SinglePoint) {
    using MyPoint = typename TestFixture::MyPoint;
    TypeParam algo;
    std::vector<size_t> pred;
    std::vector<MyPoint> single_pt{MyPoint{42.0}};
    algo(single_pt, pred);
    ASSERT_EQ(pred.size(), 1);
    EXPECT_EQ(single_pt[0], MyPoint{42.0});
}

TYPED_TEST_P(GreedyTest, TwoPoints) {
    using MyPoint = typename TestFixture::MyPoint;
    TypeParam algo;
    std::vector<size_t> pred;
    std::vector<MyPoint> two_pts{MyPoint{-1.0}, MyPoint{3.0}};
    algo(two_pts, pred);
    ASSERT_EQ(pred.size(), 2);
    std::vector<double> found_coords{two_pts[0].coords[0], two_pts[1].coords[0]};
    std::vector<double> exp_coords{-1.0, 3.0};
    EXPECT_EQ(found_coords, exp_coords);
}

TYPED_TEST_P(GreedyTest, SimpleGreedy) {
    using MyPoint = typename TestFixture::MyPoint;
    TypeParam algo;
    std::vector<MyPoint> pts, exp_gp;
    for(int i = 0; i < 3; i++)
        pts.push_back(MyPoint{double(i)});
    std::vector<size_t> pred;
    exp_gp = std::vector<MyPoint>({pts[0], pts[2], pts[1]});
    
    algo(pts, pred);
    
    EXPECT_EQ(pts, exp_gp);
}

TYPED_TEST_P(GreedyTest, ExponentialGreedy) {
    using MyPoint = typename TestFixture::MyPoint;
    TypeParam algo;
    std::vector<MyPoint> pts, exp_gp;
    for(int i = 0; i < 100; i++)
        pts.push_back(MyPoint{std::pow(-3, i)});
    
    exp_gp = std::vector<MyPoint>({pts[0], pts[99]});
    for(int i = 98; i > 0; i--)
        exp_gp.push_back(pts[i]);
    
    std::vector<size_t> pred;
    algo(pts, pred);
    EXPECT_EQ(pts, exp_gp);
}

TYPED_TEST_P(GreedyTest, Random) {
    using MyPoint = typename TestFixture::MyPoint;
    TypeParam algo;
    std::vector<double> coords{0, 8, 12, 100, 40, 70, 1, 72}, gp_coords;
    std::vector<MyPoint> pts;
    for(auto& p: coords)
        pts.push_back(MyPoint{p});
    std::vector<size_t> pred;
    algo(pts, pred);
    for(auto& p: pts)
        gp_coords.push_back(p.coords[0]);
    std::vector<double> exp_coords{0, 100, 40, 70, 12, 8, 72, 1};

    EXPECT_EQ(gp_coords, exp_coords);
}

TYPED_TEST_P(GreedyTest, PlanarPointsGP) {
    using PlanarPoint = Point<2, L1Metric>;
    TypeParam algo;
    std::vector<PlanarPoint> pts, exp_gp;
    pts.push_back(PlanarPoint({0, 0}));
    pts.push_back(PlanarPoint({1, 2}));
    pts.push_back(PlanarPoint({5, 6}));
    pts.push_back(PlanarPoint({15, 0}));
    pts.push_back(PlanarPoint({8, 5}));
    
    exp_gp.push_back(pts[0]);      // 0, 0
    exp_gp.push_back(pts[3]);      // 15, 0
    exp_gp.push_back(pts[4]);      // 8, 5
    exp_gp.push_back(pts[2]);      // 5, 6
    exp_gp.push_back(pts[1]);      // 1, 2
    
    vector<size_t> pred;
    
    algo(pts, pred);

    EXPECT_EQ(pts, exp_gp);
}

TYPED_TEST_P(GreedyTest, PlanarPointsPred) {
    using PlanarPoint = Point<2, L1Metric>;
    TypeParam algo;
    vector<PlanarPoint> pts;
    pts.push_back(PlanarPoint({0, 0}));
    pts.push_back(PlanarPoint({1, 2}));
    pts.push_back(PlanarPoint({5, 6}));
    pts.push_back(PlanarPoint({15, 0}));
    pts.push_back(PlanarPoint({8, 5}));
    
    vector<size_t> pred, exp_pred;

    exp_pred.push_back(-1);
    exp_pred.push_back(0);
    exp_pred.push_back(1);
    exp_pred.push_back(2);
    exp_pred.push_back(0);
    
    
    algo(pts, pred);

    // for(auto& p: pred)
    //     if(p)
    //         pred_pts.push_back(*p);

    EXPECT_EQ(pred, exp_pred);
}

TYPED_TEST_P(GreedyTest, SpatialPointsGP) {
    using SpatialPoint = Point<3, L2Metric>;
    TypeParam algo;

    vector<SpatialPoint> pts, exp_gp;
    pts.push_back(SpatialPoint({0, 0, 5}));
    pts.push_back(SpatialPoint({15, 0, 10}));
    pts.push_back(SpatialPoint({1, 3, 3}));
    pts.push_back(SpatialPoint({5, 6, 9}));
    pts.push_back(SpatialPoint({8, 5, 1}));
    
    vector<size_t> pred;

    exp_gp.push_back(pts[0]);      // 0, 0, 5
    exp_gp.push_back(pts[1]);      // 15, 0, 10
    exp_gp.push_back(pts[4]);      // 8, 5, 1
    exp_gp.push_back(pts[3]);      // 5, 6, 9
    exp_gp.push_back(pts[2]);      // 1, 3, 3

    algo(pts, pred);

    EXPECT_EQ(pts, exp_gp);
}

TYPED_TEST_P(GreedyTest, SpatialPointsPred) {
    using SpatialPoint = Point<3, L2Metric>;
    TypeParam algo;
    vector<SpatialPoint> pts;
    pts.push_back(SpatialPoint({0, 0, 5}));
    pts.push_back(SpatialPoint({1, 3, 3}));
    pts.push_back(SpatialPoint({5, 6, 9}));
    pts.push_back(SpatialPoint({15, 0, 10}));
    pts.push_back(SpatialPoint({8, 5, 1}));
    
    vector<size_t> exp_pred, pred;

    exp_pred.push_back(-1);
    exp_pred.push_back(0);
    exp_pred.push_back(0);
    exp_pred.push_back(2);
    exp_pred.push_back(0);

    algo(pts, pred);

    // for(auto& p: pred)
    //     if(p)
    //         pred_pts.push_back(*p);

    EXPECT_EQ(pred, exp_pred);
}

// Register all test cases
REGISTER_TYPED_TEST_SUITE_P(
    GreedyTest,
    Empty,
    SinglePoint,
    TwoPoints,
    SimpleGreedy,
    ExponentialGreedy,
    Random,
    PlanarPointsGP,
    PlanarPointsPred,
    SpatialPointsGP,
    SpatialPointsPred
);

// Instantiate with your algorithms
typedef ::testing::Types<GonzalezAlgo, ClarksonAlgo> GreedyAlgos;
INSTANTIATE_TYPED_TEST_SUITE_P(AllGreedyAlgos, GreedyTest, GreedyAlgos);
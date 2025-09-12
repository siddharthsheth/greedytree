// Type-parameterized tests for greedy clustering algorithms (Gonzalez and Clarkson)
#include <gtest/gtest.h>
#include "../include/greedy.hpp"
#include <vector>
#include <cmath>
#include <limits>

struct GonzalezAlgo {
    template <std::size_t d, typename Metric>
    void operator()(std::vector<Point<d, Metric>>& pts,
                    std::vector<const Point<d, Metric>*>& gp,
                    std::vector<const Point<d, Metric>*>& pred) const {
        gonzalez(pts, gp, pred);
    }
};
struct ClarksonAlgo {
    template <std::size_t d, typename Metric>
    void operator()(std::vector<Point<d, Metric>>& pts,
                    std::vector<const Point<d, Metric>*>& gp,
                    std::vector<const Point<d, Metric>*>& pred) const {
        clarkson(pts, gp, pred);
    }
};

// Test fixture template
template <typename Algo>
class GreedyParamTest : public ::testing::Test {
public:
    using MyPoint = Point<1, L1Metric>;
    Algo algo;
};

TYPED_TEST_SUITE_P(GreedyParamTest);

TYPED_TEST_P(GreedyParamTest, Trivial) {
    using MyPoint = typename TestFixture::MyPoint;
    TypeParam algo;
    std::vector<MyPoint> empty_pts;
    std::vector<const MyPoint*> gp, pred;
    EXPECT_NO_THROW(algo(empty_pts, gp, pred));
    EXPECT_TRUE(gp.empty());
    EXPECT_TRUE(pred.empty());
    std::vector<MyPoint> single_pt{MyPoint{42.0}};
    gp.clear(); pred.clear();
    algo(single_pt, gp, pred);
    ASSERT_EQ(gp.size(), 1);
    EXPECT_EQ(gp[0], &single_pt[0]);
    std::vector<MyPoint> two_pts{MyPoint{-1.0}, MyPoint{3.0}};
    gp.clear(); pred.clear();
    algo(two_pts, gp, pred);
    ASSERT_EQ(gp.size(), 2);
    std::vector<double> found_coords{gp[0]->coords[0], gp[1]->coords[0]};
    std::vector<double> exp_coords{-1.0, 3.0};
    EXPECT_EQ(found_coords, exp_coords);
}

TYPED_TEST_P(GreedyParamTest, SimpleGreedy) {
    using MyPoint = typename TestFixture::MyPoint;
    TypeParam algo;
    std::vector<MyPoint> pts;
    for(int i = 0; i < 3; i++)
        pts.push_back(MyPoint{double(i)});
    std::vector<const MyPoint*> gp, pred, exp_gp = {
        &pts[0], &pts[2], &pts[1]
    };
    algo(pts, gp, pred);
    EXPECT_EQ(gp, exp_gp);
}

TYPED_TEST_P(GreedyParamTest, ExponentialGreedy) {
    using MyPoint = typename TestFixture::MyPoint;
    TypeParam algo;
    std::vector<MyPoint> pts;
    for(int i = 0; i < 100; i++)
        pts.push_back(MyPoint{std::pow(-3, i)});
    std::vector<const MyPoint*> gp, pred, exp_gp = {&pts[0], &pts[99]};
    algo(pts, gp, pred);
     for(int i = 98; i > 0; i--)
        exp_gp.push_back(&pts[i]);
    EXPECT_EQ(gp, exp_gp);
}

TYPED_TEST_P(GreedyParamTest, Random) {
    using MyPoint = typename TestFixture::MyPoint;
    TypeParam algo;
    std::vector<double> coords{0, 8, 12, 100, 40, 70, 1, 72}, gp_coords;
    std::vector<MyPoint> pts;
    for(auto& p: coords)
        pts.push_back(MyPoint{p});
    std::vector<const MyPoint*> gp, pred;
    algo(pts, gp, pred);
    for(auto& p: gp)
        gp_coords.push_back(p->coords[0]);
    std::vector<double> exp_coords{0, 100, 40, 70, 12, 8, 72, 1};

    EXPECT_EQ(gp_coords, exp_coords);
}

TYPED_TEST_P(GreedyParamTest, PlanarPointsGP) {
    using PlanarPoint = Point<2, L1Metric>;
    TypeParam algo;
    std::vector<PlanarPoint> pts;
    pts.push_back(PlanarPoint({0, 0}));
    pts.push_back(PlanarPoint({1, 2}));
    pts.push_back(PlanarPoint({5, 6}));
    pts.push_back(PlanarPoint({15, 0}));
    pts.push_back(PlanarPoint({8, 5}));
    
    vector<const PlanarPoint*> gp, pred, exp_pred;
    exp_pred.push_back(nullptr);
    exp_pred.push_back(&pts[0]);
    exp_pred.push_back(&pts[3]);
    exp_pred.push_back(&pts[4]);
    exp_pred.push_back(&pts[0]);

    algo(pts, gp, pred);

    EXPECT_EQ(pred, exp_pred);
}

TYPED_TEST_P(GreedyParamTest, PlanarPointsPred) {
    using PlanarPoint = Point<2, L1Metric>;
    TypeParam algo;
    vector<PlanarPoint> pts;
    pts.push_back(PlanarPoint({0, 0}));
    pts.push_back(PlanarPoint({1, 2}));
    pts.push_back(PlanarPoint({5, 6}));
    pts.push_back(PlanarPoint({15, 0}));
    pts.push_back(PlanarPoint({8, 5}));

    vector<const PlanarPoint*> gp, pred, exp_pred;

    exp_pred.push_back(nullptr);
    exp_pred.push_back(&pts[0]);
    exp_pred.push_back(&pts[3]);
    exp_pred.push_back(&pts[4]);
    exp_pred.push_back(&pts[0]);

    algo(pts, gp, pred);

    EXPECT_EQ(pred, exp_pred);
}

TYPED_TEST_P(GreedyParamTest, SpatialPointsGP) {
    using SpatialPoint = Point<3, L2Metric>;
    TypeParam algo;

    vector<SpatialPoint> pts;
    pts.push_back(SpatialPoint({0, 0, 5}));
    pts.push_back(SpatialPoint({15, 0, 10}));
    pts.push_back(SpatialPoint({1, 3, 3}));
    pts.push_back(SpatialPoint({5, 6, 9}));
    pts.push_back(SpatialPoint({8, 5, 1}));
    
    vector<const SpatialPoint*> gp, pred, exp_gp;

    exp_gp.push_back(&pts[0]);      // 0, 0, 5
    exp_gp.push_back(&pts[1]);      // 15, 0, 10
    exp_gp.push_back(&pts[4]);      // 8, 5, 1
    exp_gp.push_back(&pts[3]);      // 5, 6, 9
    exp_gp.push_back(&pts[2]);      // 1, 3, 3

    algo(pts, gp, pred);

    EXPECT_EQ(gp, exp_gp);
}

TYPED_TEST_P(GreedyParamTest, SpatialPointsPred) {
    using SpatialPoint = Point<3, L2Metric>;
    TypeParam algo;
    vector<SpatialPoint> pts;
    pts.push_back(SpatialPoint({0, 0, 5}));
    pts.push_back(SpatialPoint({1, 3, 3}));
    pts.push_back(SpatialPoint({5, 6, 9}));
    pts.push_back(SpatialPoint({15, 0, 10}));
    pts.push_back(SpatialPoint({8, 5, 1}));
    
    vector<const SpatialPoint*> gp, pred, exp_pred;

    exp_pred.push_back(nullptr);
    exp_pred.push_back(&pts[0]);
    exp_pred.push_back(&pts[0]);
    exp_pred.push_back(&pts[4]);
    exp_pred.push_back(&pts[0]);

    algo(pts, gp, pred);

    EXPECT_EQ(pred, exp_pred);
}

// Register all test cases
REGISTER_TYPED_TEST_SUITE_P(
    GreedyParamTest,
    Trivial,
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
INSTANTIATE_TYPED_TEST_SUITE_P(AllGreedyAlgos, GreedyParamTest, GreedyAlgos);
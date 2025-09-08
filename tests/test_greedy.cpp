#include <gtest/gtest.h>
#include <vector>
#include <cmath>
#include "../include/greedy.hpp"


TEST(GreedyTest, SimpleGreedy){
    using MyPoint = Point<1, L1Metric>;
    
    std::vector<MyPoint> pts;
    for(int i = 0; i < 3; i++)
        pts.push_back(MyPoint{double(i)});
    std::vector<const MyPoint*> gp, pred, exp_gp = {
        &pts[0], &pts[2], &pts[1]
    };
    
    clarkson(pts, gp, pred);
    
    EXPECT_EQ(gp, exp_gp);
}

TEST(GreedyTest, ExponentialGreedy){
    using MyPoint = Point<1, L1Metric>;
    
    std::vector<MyPoint> pts;
    for(int i = 0; i < 100; i++)
        pts.push_back(MyPoint{std::pow(-3, i)});
    std::vector<const MyPoint*> gp, pred, exp_gp = {&pts[0], &pts[99]};
    for(int i = 98; i > 0; i--)
        exp_gp.push_back(&pts[i]);
    
    clarkson(pts, gp, pred);
    
    EXPECT_EQ(gp, exp_gp);
}

TEST(GreedyTest, Random){
    using MyPoint = Point<1, L1Metric>;
    
    std::vector<double> coords{0, 8, 12, 100, 40, 70, 1, 72}, gp_coords;
    std::vector<MyPoint> pts;
    for(auto& p: coords)
        pts.push_back(MyPoint{p});
    std::vector<const MyPoint*> gp, pred, exp_gp;
    
    clarkson(pts, gp, pred);

    for(auto& p: gp)
        gp_coords.push_back(p->coords[0]);
    std::vector<double> exp_coords{0, 100, 40, 70, 12, 8, 72, 1};

    EXPECT_EQ(gp_coords, exp_coords);
}

TEST(GreedyTest, PlanarPointsGP){
    using PlanarPoint = Point<2, L1Metric>;
    
    vector<PlanarPoint> pts;
    pts.push_back(PlanarPoint({0, 0}));
    pts.push_back(PlanarPoint({1, 2}));
    pts.push_back(PlanarPoint({5, 6}));
    pts.push_back(PlanarPoint({15, 0}));
    pts.push_back(PlanarPoint({8, 5}));

    vector<const PlanarPoint*> gp, pred, exp_gp;

    exp_gp.push_back(&pts[0]);      // 0, 0
    exp_gp.push_back(&pts[3]);      // 15, 0
    exp_gp.push_back(&pts[4]);      // 8, 5
    exp_gp.push_back(&pts[2]);      // 5, 6
    exp_gp.push_back(&pts[1]);      // 1, 2

    clarkson(pts, gp, pred);

    EXPECT_EQ(gp, exp_gp);
}

TEST(GreedyTest, PlanarPointsPred){
    using PlanarPoint = Point<2, L1Metric>;
    
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

    clarkson(pts, gp, pred);

    EXPECT_EQ(pred, exp_pred);
}

TEST(GreedyTest, SpatialPointsGP){
    using SpatialPoint = Point<3, L2Metric>;

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

    clarkson(pts, gp, pred);

    EXPECT_EQ(gp, exp_gp);
}

TEST(GreedyTest, SpatialPointsPred){
    using SpatialPoint = Point<3, L2Metric>;
    
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

    clarkson(pts, gp, pred);

    EXPECT_EQ(pred, exp_pred);
}
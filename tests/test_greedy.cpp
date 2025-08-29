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
    // exp_gp.push_back(&pts[0]);      // 0
    // exp_gp.push_back(&pts[3]);      // 100
    // exp_gp.push_back(&pts[4]);      // 40
    // exp_gp.push_back(&pts[5]);      // 70
    // exp_gp.push_back(&pts[2]);      // 12
    // exp_gp.push_back(&pts[1]);      // 8
    // exp_gp.push_back(&pts[7]);      // 72
    // exp_gp.push_back(&pts[6]);      // 1

    EXPECT_EQ(gp_coords, exp_coords);
}
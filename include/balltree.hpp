#ifndef BALLTREE_H
#define BALLTREE_H

#include "greedy.hpp"
#include <stack>

template<size_t d, typename Metric>
class BallTree {
public:
    using pt_ptr = std::shared_ptr<const Point<d, Metric>>;
    using ball_ptr = std::shared_ptr<BallTree<d, Metric>>;

private:
    struct BallRadiusCompare {
        bool operator()(const ball_ptr& a, const ball_ptr& b) const {
            return a->radius < b->radius; // max-heap
        }
    };

public:
    pt_ptr center;
    double radius;
    size_t _size;
    ball_ptr left;
    ball_ptr right;

    BallTree(pt_ptr p);
    bool isleaf();
    double dist(pt_ptr p);
    priority_queue<ball_ptr, vector<ball_ptr>, BallRadiusCompare> heap();
};

template<size_t d, typename Metric>
using BallTreePtr = std::shared_ptr<BallTree<d, Metric>>;

#include<balltree_impl.hpp>

#endif
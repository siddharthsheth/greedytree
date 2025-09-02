#ifndef BALLTREE_H
#define BALLTREE_H

#include "greedy.hpp"
#include <stack>

template<size_t d, typename Metric>
class BallTree {
public:    
    struct BallTreeCompare {
        bool operator()(const BallTree*& a, const BallTree*& b) const {
            return a->radius < b->radius; // max-heap
        }
    };

    using PtPtr = Point<d, Metric>*;
    using BallTreeUPtr = std::unique_ptr<BallTree<d, Metric>>;
    using BallHeap = std::priority_queue<
                                BallTree<d, Metric>*,
                                std::vector<BallTree<d, Metric>*>,
                                BallTreeCompare
                            >;
    
    PtPtr center;
    double radius;
    size_t _size;
    BallTreeUPtr left;
    BallTreeUPtr right;

    BallTree(const PtPtr p);
    bool isleaf();
    double dist(PtPtr p);
    BallHeap heap();
};

template<size_t d, typename Metric>
using BallTreeUPtr = std::unique_ptr<BallTree<d, Metric>>;

template <std::size_t d, typename Metric>
using PtPtrVec = std::vector<const Point<d, Metric>*>;

template<size_t d, typename Metric>
using BallHeap = std::priority_queue<
                                    BallTree<d, Metric>*,
                                    vector<BallTree<d, Metric>*>,
                                    typename BallTree<d, Metric>::BallTreeCompare
                                >;

#include<balltree_impl.hpp>

#endif
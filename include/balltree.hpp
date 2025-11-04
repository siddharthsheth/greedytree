/**
 * @file balltree.hpp
 * @author Siddarth Sheth
 * @brief BallTree data structure for efficient spatial queries in metric spaces.
 *
 * Defines the BallTree class template and related types for hierarchical clustering
 * and nearest neighbor search. 
 */

#ifndef BALLTREE_H
#define BALLTREE_H

#include "greedy.hpp"
#include <stack>
#include <unordered_map>
#include <deque>
#include <boost/unordered/unordered_flat_set.hpp>
#include <cmath>

/**
 * @brief BallTree node for hierarchical spatial partitioning.
 *
 * @tparam d Dimensionality of the space.
 * @tparam Metric Metric type for distance calculations.
 *
 * Each BallTree node represents a ball (center and radius) containing a subset of points.
 * Nodes may have left/right children for recursive partitioning.
 */
template<size_t d, typename Metric>
class BallTree {
public:
    /**
     * @brief Comparator for BallTree pointers based on radius (for max-heap usage).
     */
    struct BallTreeCompare {
        /**
         * @brief Compare two BallTree nodes by radius for heap ordering.
         * @param a Pointer to first BallTree node.
         * @param b Pointer to second BallTree node.
         * @return True if a's radius is less than b's (max-heap).
         */
        bool operator()(const BallTree* a, const BallTree* b) const {
            return a->radius < b->radius; // max-heap
        }
    };

    using Pt = std::array<double, d>;
    /**
     * @brief Pointer to a Point in d-dimensional space.
     */
    using PtPtr = const Pt*;
    /**
     * @brief Unique pointer to a BallTree node.
     */
    using BallTreeUPtr = std::unique_ptr<BallTree<d, Metric>>;
    using BallTreePtr = BallTree<d, Metric>*;
    /**
     * @brief Max-heap of BallTree pointers, ordered by radius.
     */
    using BallHeap = std::priority_queue<
                                BallTree<d, Metric>*,
                                std::vector<BallTree<d, Metric>*>,
                                BallTreeCompare
                            >;
    
    /**
     * @brief Center point of the ball.
     */
    PtPtr center;
    /**
     * @brief Radius of the ball (distance from center to farthest point).
     */
    double radius;
    /**
     * @brief Number of points contained in this ball.
     */
    size_t size;
    /**
     * @brief Left child (sub-ball) in the tree.
     */
    BallTreeUPtr left;
    /**
     * @brief Right child (sub-ball) in the tree.
     */
    BallTreeUPtr right;

    Metric metric;

    /**
     * @brief Construct a BallTree node with a given center point.
     * @param p Pointer to the center point.
     */
    BallTree(PtPtr& p, Metric metric);
    /**
     * @brief Check if this node is a leaf (no children).
     * @return True if leaf node, false otherwise.
     */
    bool isleaf();
    /**
     * @brief Compute the distance from the center to a given point.
     * @param p Pointer to the point.
     * @return Distance from center to point.
     */
    double dist(PtPtr p);
    /**
     * @brief Create a max-heap of BallTree nodes rooted at this node.
     * @return BallHeap containing nodes ordered by radius.
     */
    BallHeap heap();

    vector<PtPtr> points();

    PtPtr nearest(PtPtr query);
    PtPtr farthest(PtPtr query);
    vector<BallTree*> range(PtPtr query, double q_radius);
    
    template<typename Update, typename ViableCondition>
    void generic_search(Update update, ViableCondition is_viable);

    struct HeapOrderEntry{
        Pt center;
        double radius;
        size_t parent_index;
        double left_radius;
    };

    void get_traversal(vector<HeapOrderEntry>& output);
    void get_traversal(vector<BallTreePtr>& output);
};

/**
 * @brief Type alias for unique pointer to BallTree node.
 */
template<size_t d, typename Metric>
using BallTreeUPtr = std::unique_ptr<BallTree<d, Metric>>;

/**
 * @brief Type alias for vector of constant Point pointers.
 */
template <std::size_t d>
using PtVec = std::vector<std::array<double, d>>;

/**
 * @brief Type alias for max-heap of BallTree pointers, ordered by radius.
 */
template<size_t d, typename Metric>
using BallHeap = std::priority_queue<
                                    BallTree<d, Metric>*,
                                    vector<BallTree<d, Metric>*>,
                                    typename BallTree<d, Metric>::BallTreeCompare
                                >;

template<size_t d, typename Metric>
BallTreeUPtr<d, Metric> greedy_tree(PtVec<d>& pts, Metric metric);

#include<balltree_impl.hpp>

#endif
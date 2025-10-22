/**
 * @file neighborgraph.hpp
 * @author Siddarth Sheth
 * @brief NeighborGraph class for managing cell connectivity using Boost Graph.
 *
 * Defines the NeighborGraph class template and related types for representing
 * and manipulating a graph of cells in a metric space. Uses Boost's adjacency_list
 * for efficient graph operations.
 */

#ifndef NEIGHBORGRAPH_H
#define NEIGHBORGRAPH_H

#include "cell.hpp"
#include <queue>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <boost/unordered/unordered_flat_set.hpp>

/**
 * @brief Graph of cells for neighbor relationships in metric space.
 *
 * @tparam d Dimensionality of the space.
 * @tparam Metric Metric type for distance calculations.
 *
 * Adjacency list to represent undirected connectivity between cells.
 */
template<size_t d, typename Metric>
class NeighborGraph {
private:
    /**
     * @brief Point type in d-dimensional space.
     */
    using Pt = std::array<double, d>;
    /**
     * @brief Reference to a Cell.
     */
    using CellRef = Cell<d,Metric>&;
    
public:
    std::vector<Cell<d,Metric>> cells;
    
    /**
     * @brief Get the top cell from the heap.
     * @return Index (in cells) to the top cell.
     */
    size_t heap_top();
    
    /**
     * @brief Construct a NeighborGraph from a vector of points.
     * @param P Vector of points to initialize the graph.
     */
    NeighborGraph(std::vector<Pt>& pts, Metric metric);
    
    /**
     * @brief Add a new cell to the graph.
     */
    void add_cell();
    
    void get_permutation(bool move, std::vector<Pt>& output);
    
private:
    /**
     * @brief Pair of unique_ptr to Cell and a double (for heap operations).
     */
    using HeapPair = std::pair<size_t, double>;
    Metric metric;
    
    std::vector<size_t> affected_cells;
    std::vector<double> a_distances;
    std::vector<Pt> move_pts, keep_pts;
    std::vector<double> move_dists, keep_dists;
    std::vector<size_t> move_idx, keep_idx;
    bool centers_moved;
    
    /**
     * @brief Add an edge between two cells in the graph.
     * @param i Index of first cell in cells.
     * @param j Index of second cell in cells.
     */
    inline void add_edge(size_t i, size_t j){
        cells[i].nbrs.push_back(j);
        cells[j].nbrs.push_back(i);
    }

    /**
     * @brief Comparator for heap operations on HeapPair.
     */
    struct CellCompare {
        /**
         * @brief Compare two HeapPairs for heap ordering.
         * @param a First HeapPair.
         * @param b Second HeapPair.
         * @return True if a < b for heap purposes.
         */
        bool operator()(const HeapPair& a, const HeapPair& b)const;
    };

    /**
     * @brief Vector of HeapPairs for heap-based operations.
     */
    std::priority_queue<HeapPair, std::vector<HeapPair>, CellCompare> cell_heap;
    
    /**
     * @brief Rebalance the graph after cell changes.
     * @param a Pointer to first cell.
     * @param b Pointer to second cell.
     */
    void rebalance(size_t i, size_t j);
    /**
     * @brief Check if two cells are close enough (according to some metric).
     * @param i Index of first cell in cells.
     * @param j Index of second cell in cells.
     * @return True if close enough, false otherwise.
     */
    inline bool is_close_enough(const size_t i, const size_t j) const{
        double i_r = cells[i].radius;
        double j_r = cells[j].radius;
        double min_r = min(i_r, j_r);
        double max_r = max(i_r, j_r);
        return min_r > 0 && metric.dist(cells[i].center, cells[j].center) <= i_r + j_r + max_r;
        // return min_r > 0 && cells[i].dist(cells[j]) <= i_r + j_r + max_r;
        // return cells[i].dist(cells[j]) <= i_r + j_r + max_r;
    }

    inline bool is_close_enough(const size_t i, const size_t j, double r) const{
        return metric.dist(cells[i].center, cells[j].center) <= 3*r;
    }

    inline std::pair<size_t, size_t> init_new_cell();
    inline void point_location(size_t cell_i, size_t par_i);
    inline void nbr_nbr_update(size_t cell_i);
    inline void prune_edges();
};

#include "neighborgraph_impl.hpp"

#endif // NEIGHBORGRAPH_H
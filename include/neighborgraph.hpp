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
#include <set>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

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
public:
    /**
     * @brief Point type in d-dimensional space.
     */
    using Pt = Point<d, Metric>;
    /**
     * @brief Pointer to a Point.
     */
    using PtPtr = Point<d, Metric>*;
    /**
     * @brief Pointer to a Cell.
     */
    using CellPtr = Cell<d, Metric>*;
    using CellRef = Cell<d,Metric>&;
    
    /**
     * @brief Pair of unique_ptr to Cell and a double (for heap operations).
     */
    // using HeapPair = std::pair<std::unique_ptr<Cell<d, Metric>>, double>;
    // using HeapPair = std::pair<CellPtr, double>;
    using HeapPair = std::pair<size_t, double>;
    

    // using PtLoc = std::pair<CellPtr, size_t>;
    std::vector<Cell<d,Metric>> cells;

private:

    std::vector<size_t> affected_cells;
    
    /**
     * @brief Add an edge between two cells in the graph.
     * @param a Pointer to first cell.
     * @param b Pointer to second cell.
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

public:
    std::vector<Pt> permutation;

    /**
     * @brief Vector of HeapPairs for heap-based operations.
     */
    std::priority_queue<HeapPair, std::vector<HeapPair>, CellCompare> cell_heap;
    /**
     * @brief Comparator instance for heap operations.
     */
    CellCompare comparator;
    /**
     * @brief Get the top cell from the heap.
     * @return Pointer to the top cell.
     */
    // CellPtr heap_top();
    size_t heap_top();

    /**
     * @brief Construct a NeighborGraph from a vector of points.
     * @param P Vector of points to initialize the graph.
     */
    NeighborGraph(std::vector<Pt>& pts);
    /**
     * @brief Add a new cell to the graph.
     */
    void add_cell();
    /**
     * @brief Rebalance the graph after cell changes.
     * @param a Pointer to first cell.
     * @param b Pointer to second cell.
     */
    // void rebalance(CellPtr a, CellPtr b);
    void rebalance(size_t i, size_t j);
    /**
     * @brief Check if two cells are close enough (according to some metric).
     * @param a Pointer to first cell.
     * @param b Pointer to second cell.
     * @return True if close enough, false otherwise.
     */
    inline bool is_close_enough(const size_t i, const size_t j) const{
        return cells[i].dist(cells[j]) <= cells[i].radius +
                        cells[j].radius +
                        max(cells[i].radius, cells[j].radius);
    }

    inline bool is_close_enough(const size_t i, const size_t j, double r) const{
        return cells[i].dist(cells[j]) <= 3*r;
    }

    std::vector<Pt> get_permutation(bool move);
    bool centers_moved;
};

/**
 * @brief Type alias for vector of Cell pointers.
 */
template<size_t d, typename Metric>
using CellPtrVec = std::vector<Cell<d, Metric>*>;

/**
 * @brief Type alias for pointer to Cell.
 */
template<size_t d, typename Metric>
using CellPtr = Cell<d, Metric>*;

#include "neighborgraph_impl.hpp"

#endif // NEIGHBORGRAPH_H
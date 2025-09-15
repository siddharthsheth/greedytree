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
#include <boost/graph/adjacency_list.hpp>
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
    /**
     * @brief Adjacency list graph type with CellPtr as vertex property.
     */
    using Graph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::undirectedS,
        CellPtr
    >;
    /**
     * @brief Vertex descriptor type for the graph.
     */
    using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
    /**
     * @brief Pair of unique_ptr to Cell and a double (for heap operations).
     */
    using HeapPair = std::pair<std::unique_ptr<Cell<d, Metric>>, double>;

    using PtLoc = std::pair<CellPtr, size_t>;
private:
    /**
     * @brief The underlying graph.
     */
    Graph g;

    /**
     * @brief Map from CellPtr to graph vertex descriptor.
     */
    unordered_map<CellPtr, Vertex> vertex;

    /**
     * @brief Add a cell as a vertex in the graph.
     * @param c Pointer to the cell to add.
     */
    void add_vertex(CellPtr c);                 // Make lambda
    /**
     * @brief Add an edge between two cells in the graph.
     * @param a Pointer to first cell.
     * @param b Pointer to second cell.
     */
    void add_edge(CellPtr a, CellPtr b);        // Make lambda
    /**
     * @brief Update a vertex in the graph.
     * @param c Pointer to the cell to update.
     */
    void update_vertex(CellPtr c);              // Can be deleted

    std::vector<PtLoc> rev_nn;
    PtPtr root_pt;
    
    inline size_t pt_index(PtPtr p) const {
        return static_cast<size_t>(p - root_pt);
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
    /**
     * @brief Vector of HeapPairs for heap-based operations.
     */
    vector<HeapPair> cell_heap_vec;
    /**
     * @brief Comparator instance for heap operations.
     */
    CellCompare comparator;
    /**
     * @brief Get the top cell from the heap.
     * @return Pointer to the top cell.
     */
    CellPtr heap_top();

    /**
     * @brief Construct a NeighborGraph from a vector of points.
     * @param P Vector of points to initialize the graph.
     */
    NeighborGraph(vector<Pt>& P);
    /**
     * @brief Add a new cell to the graph.
     */
    void add_cell();
    /**
     * @brief Rebalance the graph after cell changes.
     * @param a Pointer to first cell.
     * @param b Pointer to second cell.
     */
    void rebalance(CellPtr a, CellPtr b);
    /**
     * @brief Get neighbors of neighbors for a given cell.
     * @param c Pointer to the cell.
     * @return Vector of neighboring cells' neighbors.
     */
    vector<CellPtr> nbrs_of_nbrs(CellPtr c);
    /**
     * @brief Prune neighbors of a given cell.
     * @param c Pointer to the cell.
     */
    void prune_nbrs(CellPtr c);
    /**
     * @brief Check if two cells are close enough (according to some metric).
     * @param a Pointer to first cell.
     * @param b Pointer to second cell.
     * @return True if close enough, false otherwise.
     */
    bool is_close_enough(const CellPtr a, const CellPtr b) const;   // Make lambda

    void swap_cells(size_t i, size_t j);                              // Make lambda
};

/**
 * @brief Type alias for vector of Cell pointers.
 */
template<size_t d, typename Metric>
using CellPtrVec = vector<Cell<d, Metric>*>;

/**
 * @brief Type alias for pointer to Cell.
 */
template<size_t d, typename Metric>
using CellPtr = Cell<d, Metric>*;

#include "neighborgraph_impl.hpp"

#endif // NEIGHBORGRAPH_H
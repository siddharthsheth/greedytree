#ifndef NEIGHBORGRAPH_H
#define NEIGHBORGRAPH_H

#include "cell.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <set>
#include <queue>
#include <unordered_map>
#include <iostream>

template<size_t d, typename Metric>
class NeighborGraph {
public:
    using Graph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::undirectedS,
        Cell<d, Metric>*
    >;
    using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
    // using pt_ptr = std::shared_ptr<const Point<d, Metric>>;
    using pt_ptr = Point<d, Metric>*;
    using cell_ptr = Cell<d, Metric>*;
    using heap_pair = pair<std::unique_ptr<Cell<d, Metric>>, double>;

private:
    Graph g;

    unordered_map<cell_ptr, Vertex> vertex;

    void add_vertex(cell_ptr c);
    void add_edge(cell_ptr a, cell_ptr b);
    void update_vertex(cell_ptr c);

    struct CellCompare {
        bool operator()(const heap_pair& a, const heap_pair& b)const;
    };

public:
    // priority_queue<heap_pair, vector<heap_pair>, CellCompare> cell_heap;
    vector<heap_pair> cell_heap_vec;
    CellCompare comparator;
    cell_ptr heap_top();

    NeighborGraph(vector<Point<d, Metric>>& P);
    void add_cell();
    void rebalance(cell_ptr a, cell_ptr b);
    vector<cell_ptr> nbrs_of_nbrs(cell_ptr c);
    void prune_nbrs(cell_ptr c);
    bool is_close_enough(const cell_ptr a, const cell_ptr b) const;
};

template<size_t d, typename Metric>
using CellPtrVec = vector<Cell<d, Metric>*>;

template<size_t d, typename Metric>
using CellPtr = Cell<d, Metric>*;

#include "neighborgraph_impl.hpp"

#endif
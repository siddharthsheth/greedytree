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
        std::shared_ptr<Cell<d, Metric>>
    >;
    using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
    using pt_ptr = std::shared_ptr<const Point<d, Metric>>;
    using cell_ptr = std::shared_ptr<Cell<d, Metric>>;
    using heap_pair = pair<cell_ptr, double>;

private:
    Graph g;

    unordered_map<cell_ptr, Vertex> vertex;

    void add_vertex(cell_ptr c);
    void add_edge(cell_ptr a, cell_ptr b);
    void update_vertex(cell_ptr c);

    struct CellCompare {
        bool operator()(const heap_pair a, const heap_pair b)const;
    };

public:
    priority_queue<heap_pair, vector<heap_pair>, CellCompare> cell_heap;
    cell_ptr heap_top();

    NeighborGraph(vector<pt_ptr> P);
    void add_cell();
    void rebalance(cell_ptr a, cell_ptr b);
    vector<cell_ptr> nbrs_of_nbrs(cell_ptr c);
    void prune_nbrs(cell_ptr c);
    bool is_close_enough(const cell_ptr a, const cell_ptr b) const;
};

template<size_t d, typename Metric>
using CellPtrVec = vector<std::shared_ptr<Cell<d, Metric>>>;

template<size_t d, typename Metric>
using CellPtr = std::shared_ptr<Cell<d, Metric>>;

#include "neighborgraph_impl.hpp"

#endif
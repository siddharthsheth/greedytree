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
    using Pt = Point<d, Metric>;
    using PtPtr = Point<d, Metric>*;
    using CellPtr = Cell<d, Metric>*;
    using Graph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::undirectedS,
        CellPtr
    >;
    using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
    using HeapPair = pair<std::unique_ptr<Cell<d, Metric>>, double>;

private:
    Graph g;

    unordered_map<CellPtr, Vertex> vertex;

    void add_vertex(CellPtr c);
    void add_edge(CellPtr a, CellPtr b);
    void update_vertex(CellPtr c);

    struct CellCompare {
        bool operator()(const HeapPair& a, const HeapPair& b)const;
    };

public:
    vector<HeapPair> cell_heap_vec;
    CellCompare comparator;
    CellPtr heap_top();

    NeighborGraph(vector<Pt>& P);
    void add_cell();
    void rebalance(CellPtr a, CellPtr b);
    vector<CellPtr> nbrs_of_nbrs(CellPtr c);
    void prune_nbrs(CellPtr c);
    bool is_close_enough(const CellPtr a, const CellPtr b) const;
};

template<size_t d, typename Metric>
using CellPtrVec = vector<Cell<d, Metric>*>;

template<size_t d, typename Metric>
using CellPtr = Cell<d, Metric>*;

#include "neighborgraph_impl.hpp"

#endif
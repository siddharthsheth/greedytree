#ifndef Cell_H
#define Cell_H

#define DEBUG

#ifdef DEBUG
#  define debug_log(x) std::cout << x << std::endl
#else
#  define debug_log(x) do {} while (0)
#endif

#include "point.hpp"
#include <vector>
#include <memory>

using namespace std;

template<size_t d, typename Metric>
class Cell {
public:
    using Pt = const Point<d, Metric>;
    using PtPtr = const Point<d, Metric>*;
    
    static int next_id;
    int id;

    PtPtr center;
    double radius;
    vector<PtPtr> points;
    PtPtr farthest;

    Cell();
    Cell(Pt& p);
    Cell(PtPtr p);
    
    double dist(Pt& p) const;
    double dist(const Cell& c) const;
    void add_point(PtPtr p);
    void update_radius();
    size_t size() const;
    bool operator==(const Cell& other) const;
};

template<size_t d, typename Metric>
struct std::hash<Cell<d, Metric>> {
    size_t operator()(const Cell<d, Metric>& c) const;
};

template<size_t d, typename Metric>
using CellHash = std::hash<Cell<d, Metric>>;

#include "cell_impl.hpp"

#endif

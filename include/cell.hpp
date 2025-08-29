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
    using pt = const Point<d, Metric>;
    using pt_ptr = const Point<d, Metric>*;
    
    static int next_id;
    int id;

    pt_ptr center;
    double radius;
    vector<pt_ptr> points;
    pt_ptr farthest;

    Cell();
    Cell(pt& p);
    Cell(pt_ptr p);
    
    double dist(pt& p) const;
    double dist(const Cell& c) const;
    void add_point(pt_ptr p);
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

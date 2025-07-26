#ifndef Cell_H
#define Cell_H

#include "point.hpp"
#include <unordered_set>
#include <memory>

using namespace std;

template<size_t d, typename Metric>
class Cell {
public:
    using pt = const Point<d, Metric>;
    using pt_ptr = std::shared_ptr<const Point<d, Metric>>;
    
    static int next_id;
    int id;

    pt_ptr center;
    double radius;
    unordered_set<pt_ptr> points;
    pt_ptr farthest;

    Cell();
    Cell(pt& p);
    Cell(pt_ptr p);
    
    double dist(pt& p) const;
    double dist(const Cell& c) const;
    void add_point(pt_ptr p);
    void remove_point(pt_ptr p);
    void update_radius();
    size_t size() const;
    bool contains(pt_ptr p) const;
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

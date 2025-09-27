template<size_t d, typename Metric>
int Cell<d, Metric>::next_id = 0;

// template <size_t d, typename Metric>
// Cell<d,Metric>::Cell(Pt p) :
//                     id(next_id++),
//                     center(std::move(p)),
//                     radius(0),
//                     farthest(0) {
//     debug_log("Cell: Created cell with center " << center);
// }

template <size_t d, typename Metric>
Cell<d,Metric>::Cell(size_t p_i, vector<Pt>& all_points) :
                    id(next_id++),
                    center(p_i),
                    radius(0),
                    farthest(p_i),
                    global_points(all_points) {
    debug_log("Cell: Created cell with center " << center);
}

template <size_t d, typename Metric>
double Cell<d,Metric>::dist(Pt& p) const {
    return global_points[center].dist(p);
}

template <size_t d, typename Metric>
double Cell<d,Metric>::dist(const Cell& c) const {
    return global_points[center].dist(global_points[c.center]);
}

template <size_t d, typename Metric>
double Cell<d,Metric>::dist(size_t p_i) const {
    return global_points[center].dist(global_points[p_i]);
}

template <size_t d, typename Metric>
double Cell<d,Metric>::compare_dist(Pt& p) const {
    return global_points[center].compare_dist(p);
}

template <size_t d, typename Metric>
double Cell<d,Metric>::compare_dist(const Cell& c) const {
    return global_points[center].compare_dist(global_points[c.center]);
}

template <size_t d, typename Metric>
double Cell<d,Metric>::compare_dist(size_t p_i) const {
    return global_points[center].compare_dist(global_points[p_i]);
}

// template <size_t d, typename Metric>
// double Cell<d,Metric>::dist(Pt& p) const {
//     return center.dist(p);
// }

// template <size_t d, typename Metric>
// double Cell<d,Metric>::dist(const Cell& c) const {
//     return center.dist(c.center);
// }

// template <size_t d, typename Metric>
// double Cell<d,Metric>::compare_dist(Pt& p) const {
//     return center.compare_dist(p);
// }

// template <size_t d, typename Metric>
// double Cell<d,Metric>::compare_dist(const Cell& c) const {
//     return center.compare_dist(c.center);
// }

template <size_t d, typename Metric>
void Cell<d,Metric>::update_radius() {
    radius = 0;
    farthest = -1;
    for (size_t i=0; i < points.size(); i++) {
        double dist_p = dist(points[i]);
        if (dist_p > radius) {
            radius = dist_p;
            farthest = points[i];
        }
    }
}

// template <size_t d, typename Metric>
// Point<d,Metric> Cell<d,Metric>:: pop_farthest(){
//     assert(!points.empty());
//     std::swap(points[farthest], points.back());
//     Pt output = std::move(points.back());
//     points.pop_back();
//     update_radius();
//     return output;
// }

template <size_t d, typename Metric>
size_t Cell<d,Metric>::size() const {
    return points.size();
}

// Compare Cells by id
template <size_t d, typename Metric>
bool Cell<d,Metric>::operator==(const Cell& other) const {
    return id == other.id;
}

// Hash by id
template <size_t d, typename Metric>
size_t CellHash<d, Metric>::operator()(const Cell<d, Metric>& c) const {
    return std::hash<int>()(c.id);
}

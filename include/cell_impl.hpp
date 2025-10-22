template<size_t d, typename Metric>
int Cell<d, Metric>::next_id = 0;

template <size_t d, typename Metric>
Cell<d,Metric>::Cell(Pt&& p, Metric metric) :
                    id(next_id++),
                    center(std::move(p)),
                    radius(0),
                    metric(metric){
    debug_log("Cell: Created cell with center " << center);
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
    if(points.empty()){
        radius = 0;
        debug_log("update_radius: The cell " << center << " has no points and its radius is 0.");
    }
    else {
        auto max_dist = std::max_element(distances.begin(), distances.end());
        size_t far_i = std::distance(distances.begin(), max_dist);
        std::swap(distances[0], distances[far_i]);
        std::swap(points[0], points[far_i]);
        radius = metric.dist(center, points[0]);
        debug_log("update_radius: Farthest point in cell " << center << " is " << points[0] << " at distance " << distances[0] << " and radius is " << radius);
    }
}

template <size_t d, typename Metric>
std::array<double, d> Cell<d,Metric>::pop_farthest(){
    assert(!points.empty());
    Pt output = std::move(points[0]);
    points[0] =std::move(points.back());
    points.pop_back();
    distances[0] = distances.back();
    distances.pop_back();
    update_radius();
    return output;
}

template <size_t d, typename Metric>
size_t Cell<d,Metric>::size() const {
    return points.size();
}

// // Compare Cells by id
// template <size_t d, typename Metric>
// bool Cell<d,Metric>::operator==(const Cell& other) const {
//     return id == other.id;
// }

// // Hash by id
// template <size_t d, typename Metric>
// size_t CellHash<d, Metric>::operator()(const Cell<d, Metric>& c) const {
//     return std::hash<int>()(c.id);
// }

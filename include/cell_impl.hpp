template<size_t d, typename Metric>
int Cell<d, Metric>::next_id = 0;

template <size_t d, typename Metric>
Cell<d,Metric>::Cell() :
                        id(next_id++),
                        center(&origin<d, Metric>),
                        radius(0),
                        farthest(&origin<d, Metric>){
    debug_log("Created cell centered at origin.");
}

template <size_t d, typename Metric>
Cell<d,Metric>::Cell(Pt& p) :
                    id(next_id++),
                    center(&p),
                    radius(0),
                    farthest(&p) {
    // points.push_back(&p);
    debug_log("Created cell with center " << *center);
}

template <size_t d, typename Metric>
Cell<d,Metric>::Cell(PtPtr p) :
                        id(next_id++),
                        center(p),
                        radius(0),
                        farthest(p) {
    // points.push_back(p);
    debug_log("Created cell with pointer center " << *center);
}

template <size_t d, typename Metric>
void Cell<d,Metric>::add_point(PtPtr p) {
    points.push_back(p);
    double dist_p = dist(*p);
    if (dist_p > radius) {
        radius = dist_p;
        farthest = p;
    }
    // debug_log("New farthest point: " << *farthest << " at distance " << dist(*farthest));
}

template <size_t d, typename Metric>
double Cell<d,Metric>::dist(Pt& p) const {
    return (*center).dist(p);
}

template <size_t d, typename Metric>
double Cell<d,Metric>::dist(const Cell& c) const {
    return (*center).dist(*(c.center));
}

template <size_t d, typename Metric>
void Cell<d,Metric>::update_radius() {
    radius = 0;
    for (auto& p : points) {
        double dist_p = dist(*p);
        if (dist_p > radius) {
            radius = dist_p;
            farthest = p;
        }
    }
}

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

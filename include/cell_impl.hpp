template<size_t d, typename Metric>
int Cell<d, Metric>::next_id = 0;

// template <size_t d, typename Metric>
// Cell<d,Metric>::Cell() :
//                         id(next_id++),
//                         center(&origin<d, Metric>),
//                         radius(0),
//                         farthest(&origin<d, Metric>){
//     debug_log("Created cell centered at origin.");
// }

template <size_t d, typename Metric>
Cell<d,Metric>::Cell(Pt& p) :
                    id(next_id++),
                    center(&p),
                    radius(0),
                    farthest(0) {
    debug_log("Created cell with center " << *center);
}

template <size_t d, typename Metric>
Cell<d,Metric>::Cell(PtPtr p) :
                        id(next_id++),
                        center(p),
                        radius(0),
                        farthest(0) {
    debug_log("Created cell with pointer center " << *center);
}

// template <size_t d, typename Metric>
// void Cell<d,Metric>::add_point(PtPtr p) {
//     points.push_back(p);
//     double dist_p = dist(*p);
//     if (dist_p > radius) {
//         radius = dist_p;
//         farthest = p;
//     }
// }

// template <size_t d, typename Metric>
// void Cell<d,Metric>::extend_points(vector<PtPtr>& pts){
//     size_t old_len = points.size();
//     points.insert(points.end(),
//                 std::make_move_iterator(pts.begin()),
//                 std::make_move_iterator(pts.end()));
//     for (auto p = points.begin()+old_len; p != points.end(); p++) {
//         double dist_p = dist(**p);
//         if (dist_p > radius) {
//             radius = dist_p;
//             farthest = *p;
//         }
//     }
// }

// template <size_t d, typename Metric>
// void Cell<d,Metric>::replace_points(vector<PtPtr>& pts){
//     size_t old_len = points.size();
//     points = std::move(pts);
//     if(points.size() < old_len)
//         update_radius();
// }

template <size_t d, typename Metric>
double Cell<d,Metric>::dist(Pt& p) const {
    return center->dist(p);
}

template <size_t d, typename Metric>
double Cell<d,Metric>::dist(const Cell& c) const {
    return center->dist(*(c.center));
}

template <size_t d, typename Metric>
double Cell<d,Metric>::compare_dist(Pt& p) const {
    return center->compare_dist(p);
}

template <size_t d, typename Metric>
double Cell<d,Metric>::compare_dist(const Cell& c) const {
    return center->compare_dist(*(c.center));
}

template <size_t d, typename Metric>
void Cell<d,Metric>::update_radius() {
    radius = 0;
    for (size_t i=0; i < points.size(); i++) {
        double dist_p = dist(points[i]);
        if (dist_p > radius) {
            radius = dist_p;
            farthest = i;
        }
    }
}

template <size_t d, typename Metric>
Point<d,Metric> Cell<d,Metric>:: pop_farthest(){
    std::swap(points[farthest], points.back());
    Pt output = std::move(points.back());
    points.pop_back();
    return output;
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

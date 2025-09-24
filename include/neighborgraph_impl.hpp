template <std::size_t d, typename Metric>
NeighborGraph<d, Metric>::NeighborGraph(vector<Pt>& pts): centers_moved(false){

    cells.reserve(pts.size());
    std::swap(pts.front(), pts.back());
    Pt root_pt = std::move(pts.back());
    pts.pop_back();
    
    cells.push_back(Cell(std::move(root_pt)));
    CellRef root = cells[0];

    root.points = std::move(pts);
    root.update_radius();
    root.nbrs.push_back(0);
    
    cell_heap.push(HeapPair({0, root.radius}));

    debug_log("NeighborGraph: Root cell created.");
}

// template <std::size_t d, typename Metric>
// bool NeighborGraph<d, Metric>::is_close_enough(const CellPtr a, const CellPtr b) const{
//     if( a->dist(*b) <= a->radius + b->radius + max(a->radius, b->radius))
//         debug_log("Allowing: (" << *(a->center) << ", " << *(b->center) << ") as d(a,b) = "
//             << a->dist(*b) << " <= " << a->radius << " + " << b->radius
//             << " + " << max(a->radius, b->radius) << " = "
//             << a->radius + b->radius + max(a->radius, b->radius));
//     else
//         debug_log("Ignoring: (" << *(a->center) << ", " << *(b->center) << ") as d(a,b) = "
//             << a->dist(*b) << " > " << a->radius << " + " << b->radius
//             << " + " << max(a->radius, b->radius) << " = "
//             << a->radius + b->radius + max(a->radius, b->radius));
//     return a->dist(*b) <= a->radius + b->radius + max(a->radius, b->radius);
// }

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::add_cell(){
    if(centers_moved){
        debug_log("add_cell: Cells do not exist");
        return;
    }
    size_t par = heap_top();

    Pt center = std::move(cells[par].pop_farthest());
    debug_log("add_cell: New center is " << center);
    cells.push_back(Cell(std::move(center)));
    CellRef newcell = cells.back();
    size_t newcell_i = cells.size()-1;
    newcell.nbrs.push_back(newcell_i);
    
    affected_cells.clear();
    for(size_t i: cells[par].nbrs)
        rebalance(newcell_i, i);
    affected_cells.push_back(par);
    newcell.update_radius();
    
    debug_log("add_cell: Finding nbrs of nbrs of " << cells[par].center);
    // for nbrs_of_nbrs, restrict to affected_nbrs
    nbrs.clear();
    for(size_t i: affected_cells)
        for(size_t j: cells[i].nbrs)
            if(is_close_enough(newcell_i, j))
                nbrs.insert(j);
    
    newcell.nbrs.reserve(nbrs.size());
    for(size_t i: nbrs){
        debug_log("add_cell: Adding edge between " << cells[i].center << " and " << newcell.center);
        add_edge(newcell_i, i);
    }
    
    debug_log("add_cell: Pruning");
    // prune only affected_nbrs
    for(size_t i: affected_cells){
        auto iter = std::partition(cells[i].nbrs.begin(), cells[i].nbrs.end(), 
                [&](const size_t j){
                    return is_close_enough(i, j);
                });
        cells[i].nbrs.erase(iter, cells[i].nbrs.end());
    }
    
    debug_log("add_cell: Adding cell " << newcell.center << " to heap with radius " << newcell.radius);
    cell_heap.push(HeapPair({newcell_i, newcell.radius}));
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::rebalance(size_t i, size_t j){
    // instead of moving b->points, make points a reference to it
    vector<Pt>& points = cells[j].points;
    CellRef a = cells[i];
    CellRef b = cells[j];
    debug_log("rebalance: PL from " << b.center << " to " << a.center);
    auto iter = std::partition(points.begin(), points.end(), [&](Pt& p){
        if(cells[i].compare_dist(p) < cells[j].compare_dist(p)){
            debug_log("Moving " << p << " because " << cells[i].compare_dist(p) << " < " << cells[j].compare_dist(p));
        }
        else{
            debug_log("Not moving " << p << " because " << cells[i].compare_dist(p) << " >= " << cells[j].compare_dist(p));
        }
        return cells[i].compare_dist(p) >= cells[j].compare_dist(p);
    });

    if(iter != points.end()){
        // add j to affected_nbrs
        affected_cells.push_back(j);

        cells[i].points.insert(cells[i].points.end(),
                        std::make_move_iterator(iter),
                        std::make_move_iterator(points.end()));
        points.erase(iter, points.end());
        points.shrink_to_fit();

        cells[j].update_radius();
    }
}

template<size_t d, typename Metric>
bool NeighborGraph<d, Metric>::CellCompare::operator()(
                const HeapPair& a,
                const HeapPair& b
    ) const {
    auto& [a_c, a_r] = a;
    auto& [b_c, b_r] = b;
    if (a_r != b_r)
        return a_r < b_r;
    return a_c < b_c;  // unique tiebreaker
}

template<size_t d, typename Metric>
size_t NeighborGraph<d, Metric>::heap_top(){
    if(centers_moved){
        debug_log("heap_top: Cells do not exist");
        return -1;
    }
    while (!cell_heap.empty()) {
        // mutable access to the top element
        auto [i, r] = cell_heap.top();
        double c_r = cells[i].radius;
        if(r > c_r){
            // pop top moves highest priority element to back of vector
            cell_heap.pop();
            // update priority and push it back into the heap
            cell_heap.push(HeapPair({i, c_r}));
        }
        else{
            debug_log("heap_top: Getting top of heap: " << cells[i].center << " cell radius " << cells[i].radius);
            return i;
        }
    }
    return -1;
}

// template<size_t d, typename Metric>
// void NeighborGraph<d, Metric>::swap_cells(size_t i, size_t j){
//     auto [c_i, l_i] = rev_nn[i];
//     auto [c_j, l_j] = rev_nn[j];
//     auto& p = c_i->points[l_i];
//     auto& q = c_j->points[l_j];

//     if(c_i->farthest == p)
//         c_i->farthest = q;
//     if(c_j->farthest == q)
//         c_j->farthest = p;
    
//     std::swap(p, q);
//     std::swap(rev_nn[i], rev_nn[j]);
// }

template<size_t d, typename Metric>
std::vector<Point<d, Metric>> NeighborGraph<d, Metric>::get_permutation(bool move){
    std::vector<Pt> output;
    if(centers_moved){
        debug_log("get_permutation: Cells do not exist");
        return output;
    }
    output.reserve(cells.size());
    if(move){
        for(auto&c: cells)
            output.push_back(std::move(c.center));
        centers_moved = true;
    }
    else{
        for(auto&c: cells)
            output.push_back(c.center);
    }
    return output;
}
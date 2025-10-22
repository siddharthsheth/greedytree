template <std::size_t d, typename Metric>
NeighborGraph<d, Metric>::NeighborGraph(vector<Pt>& pts,
                                        Metric metric):
                                        centers_moved(false),
                                        metric(metric){

    // reserve space for vector of cells
    cells.reserve(pts.size());

    // extract seed point from input vector
    std::swap(pts.front(), pts.back());
    Pt root_pt = std::move(pts.back());
    pts.pop_back();

    std::vector<double> distances;
    std::transform(pts.begin(), pts.end(),
                    std::back_inserter(distances),
                    [&root_pt, &metric](const Pt& pt){
                        return metric.compare_dist(root_pt, pt); 
                    });
    
    // initialize root cell
    cells.push_back(Cell(std::move(root_pt), metric));
    CellRef root = cells[0];

    // point location for root cell
    root.points = std::move(pts);
    root.distances = std::move(distances);

    // radius update for root cell
    root.update_radius();

    // initialize the neighbor graph
    root.nbrs.push_back(0);
    
    // initialize the cell heap with the root cell
    cell_heap.push(HeapPair({0, root.radius}));

    debug_log("NeighborGraph: Root cell created.");
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::add_cell(){
    if(centers_moved){
        debug_log("add_cell: Cells do not exist");
        return;
    }
    
    // get the location (in cells) of the new cell and its parent
    auto [par_i, cell_i] = init_new_cell();
    // move points from the neighbors of the parent to the new cell
    point_location(cell_i, par_i);
    // discover nbrs of the new cell by looking at 2-hop nbrs of the parent
    nbr_nbr_update(cell_i);
    // prune long edges in the graph
    prune_edges();
    // add the new cell to the cell heap
    debug_log("add_cell: Adding cell " << cells[cell_i].center << " to heap with radius " << cells[cell_i].radius);
    cell_heap.push(HeapPair({cell_i, cells[cell_i].radius}));
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::rebalance(size_t i, size_t j){
    debug_log("rebalance: PL on " << cells[j].points.size() << " points from " << cells[j].center << " to " << cells[i].center);
    
    CellRef a = cells[i];
    CellRef b = cells[j];

    if(b.points.empty())
        return;

    Pt& a_center = a.center;
    
    // partition the b.points into those that stay and those that move
    // auto iter = std::partition(b.points.begin(), b.points.end(), [&](Pt& p){
    //     return a.compare_dist(p) >= b.compare_dist(p);
    // });
    // b.points.begin() ... iter should stay and iter ... b.points.end() should move
    
    // std::transform(b.points.begin(), b.points.end(),
    //                 std::back_inserter(a_distances),
    //                 [&a_center](const Pt& pt){ return a_center.compare_dist(pt); });
    // a_distances.reserve(b.points.size());
    for(Pt& pt:b.points)
        a_distances.push_back(metric.compare_dist(a_center, pt));
    
    bool farthest_moved = a_distances[0] < b.distances[0];

    size_t l_i = 0, r_i = b.points.size()-1;
    while(l_i <= r_i){
        if(a_distances[l_i] < b.distances[l_i]){
            std::swap(b.points[l_i], b.points[r_i]);
            b.distances[l_i] = b.distances[r_i];
            b.distances[r_i] = a_distances[l_i];
            a_distances[l_i] = a_distances[r_i];
            if(r_i == 0)
                break;
            r_i--;
        } else
            l_i++;
    }
    // 0 ... l_i-1 should stay and l_i ... b.points.back() should move
    // debug_log("rebalance: " << l_i << " " << r_i << " " << b.points.size());

    // if something should move
    if(l_i != b.points.size()){
        // mark the current cell as affected
        affected_cells.push_back(j);

        // move the points to a
        a.points.insert(a.points.end(),
                        std::make_move_iterator(b.points.begin()+l_i),
                        std::make_move_iterator(b.points.end()));
        a.distances.insert(a.distances.end(),
                        b.distances.begin()+l_i,
                        b.distances.end());

        // update the points of b
        b.points.erase(b.points.begin()+l_i, b.points.end());
        // b.points.shrink_to_fit();
        b.distances.erase(b.distances.begin()+l_i, b.distances.end());
        // b.distances.shrink_to_fit();
        // update the radius of b
        if(farthest_moved)
            b.update_radius();
    }
    a_distances.clear();
}

// template <std::size_t d, typename Metric>
// void NeighborGraph<d, Metric>::rebalance(size_t i, size_t j){
//     debug_log("rebalance: PL on " << cells[j].points.size() << " points from " << cells[j].center << " to " << cells[i].center);
    
//     CellRef a = cells[i];
//     CellRef b = cells[j];

//     if(b.points.empty())
//         return;

//     Pt& a_center = a.center;
    
//     // partition the b.points into those that stay and those that move
//     // auto iter = std::partition(b.points.begin(), b.points.end(), [&](Pt& p){
//     //     return a.compare_dist(p) >= b.compare_dist(p);
//     // });
//     // b.points.begin() ... iter should stay and iter ... b.points.end() should move
    
//     // std::transform(b.points.begin(), b.points.end(),
//     //                 std::back_inserter(a_distances),
//     //                 [&a_center](const Pt& pt){ return a_center.compare_dist(pt); });
//     // a_distances.reserve(b.points.size());
//     for(Pt& pt:b.points)
//         a_distances.push_back(metric.compare_dist(a_center, pt));
    
//     bool farthest_moved = a_distances[0] < b.distances[0];

//     move_idx.reserve(b.points.size());

//     for (size_t k = 0; k < b.points.size(); ++k)
//         if (a_distances[k] < b.distances[k])
//             move_idx.push_back(k);

//     // if something should move
//     if (!move_idx.empty()) {
//         affected_cells.push_back(j);

//         const size_t n_move = move_idx.size();

//         // Compact new points/distances for `a` and build a keep list for `b`
//         a.points.reserve(n_move);
//         a.distances.reserve(n_move);

//         for (size_t idx : move_idx){
//             a.points.push_back(std::move(b.points[idx]));
//             a.distances.push_back(a_distances[idx]);
//         }

//         // a.points.insert(a.points.end(),
//         //                 std::make_move_iterator(move_pts.begin()),
//         //                 std::make_move_iterator(move_pts.end()));
//         // a.distances.insert(a.distances.end(),
//         //                 move_dists.begin(),
//         //                 move_dists.end());

//         // Rebuild b.points and b.distances in place (erase gaps)
//         keep_pts.reserve(b.points.size() - n_move);
//         keep_dists.reserve(b.distances.size() - n_move);

//         for (size_t k = 0; k < b.points.size(); ++k) {
//             if (a_distances[k] >= b.distances[k]) {
//                 keep_pts.push_back(std::move(b.points[k]));
//                 keep_dists.push_back(b.distances[k]);
//             }
//         }

//         b.points.swap(keep_pts);
//         b.distances.swap(keep_dists);

//         if (farthest_moved)
//             b.update_radius();
//     }
//     a_distances.clear();
//     move_idx.clear();
//     keep_idx.clear();
//     // move_dists.clear();
//     // move_pts.clear();
//     keep_dists.clear();
//     keep_pts.clear();
// }

template <std::size_t d, typename Metric>
inline std::pair<size_t, size_t> NeighborGraph<d, Metric>::init_new_cell(){
    // get the cell at the top of the cell heap
    size_t par = heap_top();
    // extract its farthest point
    Pt center = std::move(cells[par].pop_farthest());
    
    // create new cell centered at this point
    debug_log("add_cell: New center is " << center);
    cells.push_back(Cell(std::move(center), metric));
    // add edge from new cell to itself
    size_t newcell_i = cells.size()-1;
    cells.back().nbrs.push_back(newcell_i);

    return std::pair<size_t, size_t>({par, newcell_i});
}

template <std::size_t d, typename Metric>
inline void NeighborGraph<d, Metric>::point_location(size_t cell_i, size_t par_i){
    // clear affected cells
    affected_cells.clear();
    // move points from each nbr of parent to the new cell
    for(size_t i: cells[par_i].nbrs)
        rebalance(cell_i, i);
    // compute the radius of the new cell
    cells[cell_i].update_radius();
    // no other point from parent may have moved, but even then parent is to marked as affected
    if(std::find(affected_cells.begin(), affected_cells.end(), par_i) != affected_cells.end())
        affected_cells.push_back(par_i);
}

template <std::size_t d, typename Metric>
inline void NeighborGraph<d, Metric>::nbr_nbr_update(size_t cell_i){
    debug_log("nbr_nbr_update: Finding nbrs of nbrs");

    boost::unordered_flat_set<size_t> nbrs;
    
    // for nbrs of each affected nbr of parent, check if nbr of nbr is close enough
    for(size_t i: affected_cells)
        for(size_t j: cells[i].nbrs)
            if(is_close_enough(cell_i, j))
                nbrs.insert(j);
    
    // for each viable nbr of nbr, connect it to the new cell
    debug_log("nbr_nbr_update: Nbrs discovered");
    cells[cell_i].nbrs.reserve(nbrs.size());
    for(size_t i: nbrs){
        debug_log("nbr_nbr_update: Adding edge between " << cells[i].center << " and " << cells[cell_i].center);
        add_edge(cell_i, i);
    }
}

template <std::size_t d, typename Metric>
inline void NeighborGraph<d, Metric>::prune_edges(){
    debug_log("prune_edges: Pruning long edges");
    // prune each affected nbrs
    // it should be noted that this pruning implementation is not bidirectional
    for(size_t i: affected_cells){
        // check which edges with cells[i] can be pruned
        auto iter = std::remove_if(cells[i].nbrs.begin(), cells[i].nbrs.end(), 
                [&](const size_t j){
                    return !(is_close_enough(i, j));
                });
        // now cells[i].nbrs.begin() ... iter cannot be pruned
        // so we prune the remaining edges
        cells[i].nbrs.erase(iter, cells[i].nbrs.end());
        // not calling shrink to fit as the number of edges can grow
    }
}

template<size_t d, typename Metric>
bool NeighborGraph<d, Metric>::CellCompare::operator()(
                const HeapPair& a,
                const HeapPair& b
    ) const {
    auto& [a_i, a_r] = a;
    auto& [b_i, b_r] = b;
    if (a_r != b_r)
        return a_r < b_r;
    return a_i < b_i;  // unique tiebreaker
}

template<size_t d, typename Metric>
size_t NeighborGraph<d, Metric>::heap_top(){
    if(centers_moved){
        debug_log("heap_top: Cells do not exist");
        return -1;
    }
    while (!cell_heap.empty()) {
        // access the top element
        auto [i, r] = cell_heap.top();
        // current radius of this cell
        double c_r = cells[i].radius;
        // if the radius on the heap is stale, update it
        if(r > c_r){
            // pop top moves highest priority element to back of vector
            cell_heap.pop();
            // update priority and push it back into the heap
            cell_heap.push(HeapPair({i, c_r}));
        }
        // else return it
        else{
            debug_log("heap_top: Getting top of heap: " << cells[i].center << " cell radius " << cells[i].radius);
            return i;
        }
    }
    // should never be called
    return -1;
}

template<size_t d, typename Metric>
void NeighborGraph<d, Metric>::get_permutation(bool move, std::vector<Pt>& output){
    output.clear();
    if(centers_moved){
        debug_log("get_permutation: Cells do not exist");
        return;
    }
    output.reserve(cells.size());
    // if centers are to be moved, move them
    if(move){
        for(auto&c: cells)
            output.push_back(std::move(c.center));
        centers_moved = true;
    }
    // else push a copy
    else{
        for(auto&c: cells)
            output.push_back(c.center);
    }
}
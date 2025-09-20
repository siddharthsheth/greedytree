template <std::size_t d, typename Metric>
NeighborGraph<d, Metric>::NeighborGraph(vector<Pt>& pts){

    std::swap(pts.front(), pts.back());
    permutation.reserve(pts.size());
    permutation.push_back(std::move(pts.back()));
    pts.pop_back();
    
    auto root = std::make_unique<Cell<d, Metric>>(permutation.back());

    root->points = std::move(pts);
    root->update_radius();
    (root->nbrs).push_back(root.get());
    
    cell_heap_vec.push_back(HeapPair({std::move(root), root->radius}));
    std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);

    debug_log("NeighborGraph: Root cell created.");
    // cell_heap.push(HeapPair({cells[0].get(), cells[0]->radius}));
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
    auto parent = heap_top();

    permutation.push_back(parent->pop_farthest());
    auto newcell = std::make_unique<Cell<d, Metric>>(permutation.back());
    CellPtr newcellptr = newcell.get();
    (newcellptr->nbrs).push_back(newcellptr);
    
    affected_cells.clear();
    for(CellPtr c: parent->nbrs){
        rebalance(newcellptr, c);
    }
    newcellptr->update_radius();
    parent->update_radius();
    
    debug_log("add_cell: Finding nbrs of nbrs of " << *(parent->center));
    // for nbrs_of_nbrs, restrict to affected_nbrs
    unordered_set<CellPtr> relevant_nbrs;
    for(CellPtr parent_nbr: affected_cells)
    for(CellPtr nbrptr: parent_nbr->nbrs)
    if(is_close_enough(newcellptr, nbrptr))
    relevant_nbrs.insert(nbrptr);
    
    for(CellPtr c: relevant_nbrs){
        debug_log("add_cell: Adding edge between " << *(c->center) << " and " << *(newcell->center));
        add_edge(newcellptr, c);
    }
    
    debug_log("add_cell: Pruning");
    // prune only affected_nbrs
    // include the new cell in affected_cells
    affected_cells.push_back(newcellptr);
    for(CellPtr c: affected_cells){
        auto iter = std::partition(c->nbrs.begin(), c->nbrs.end(), 
                [&](const CellPtr nbr){
                    return is_close_enough(c, nbr);
                });
        c->nbrs.erase(iter, c->nbrs.end());
    }
    
    debug_log("add_cell: Adding cell " << *(newcellptr->center) << " to heap with radius " << newcellptr->radius);
    cell_heap_vec.push_back(HeapPair({std::move(newcell), newcellptr->radius}));
    std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);
    // cell_heap.push(HeapPair({newcellptr, newcellptr->radius}));
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::rebalance(CellPtr a, CellPtr b){
    // instead of moving b->points, make points a reference to it
    vector<Pt>& points = b->points;
    debug_log("rebalance: PL from " << *(b->center) << " to " << *(a->center));
    auto b_iter = std::partition(points.begin(), points.end(), [&](Pt& p){
        return a->compare_dist(p) < b->compare_dist(p);
    });

    if(b_iter != points.begin()){
        // also add b to affected_nbrs
        affected_cells.push_back(b);

        a->points.insert(a->points.end(), points.begin(), b_iter);
        points.erase(points.begin(), b_iter);

        b->update_radius();
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
    if (!(*(a_c->center) == *(b_c->center)))
        return *(a_c->center) < *(b_c->center);
    return a_c->id < b_c->id;  // unique tiebreaker
}

template<size_t d, typename Metric>
CellPtr<d, Metric> NeighborGraph<d, Metric>::heap_top(){
    while (!cell_heap_vec.empty()) {
        // mutable access to the top element
        auto& top = cell_heap_vec.front();
        CellPtr c = top.first.get();
        // debug_log("heap_top: top.first = " << *(c->center) )
        if(top.second > c->radius){
            // pop top moves highest priority element to back of vector
            std::pop_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);

            // update priority of this element
            cell_heap_vec.back().second = c->radius;

            // push it back into the heap
            std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);
        }
        else{
            debug_log("heap_top: Getting top of heap: " << *(c->center) << " cell radius " << c->radius);
            return c;
        }
    }
    return nullptr;
}

// template<size_t d, typename Metric>
// CellPtr<d, Metric> NeighborGraph<d, Metric>::heap_top(){
//     while (!cell_heap.empty()) {
//         // mutable access to the top element
//         auto [c, r] = cell_heap.top();
//         if(r > c->radius){
//             // pop top moves highest priority element to back of vector
//             cell_heap.pop();
//             // update priority and push it back into the heap
//             cell_heap.push(HeapPair({c, c->radius}));
//         }
//         else{
//             debug_log("getting top of heap: " << *(c->center) << " cell radius " << c->radius);
//             return c;
//         }
//     }
//     return nullptr;
// }

template<size_t d, typename Metric>
void NeighborGraph<d, Metric>::swap_cells(size_t i, size_t j){
    auto [c_i, l_i] = rev_nn[i];
    auto [c_j, l_j] = rev_nn[j];
    auto& p = c_i->points[l_i];
    auto& q = c_j->points[l_j];

    if(c_i->farthest == p)
        c_i->farthest = q;
    if(c_j->farthest == q)
        c_j->farthest = p;
    
    std::swap(p, q);
    std::swap(rev_nn[i], rev_nn[j]);
}
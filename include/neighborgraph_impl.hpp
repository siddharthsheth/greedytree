template <std::size_t d, typename Metric>
NeighborGraph<d, Metric>::NeighborGraph(vector<Pt>& pts){
    root_pt = &pts[0];
    auto root = std::make_unique<Cell<d, Metric>>(pts[0]);
    CellPtr rootptr = root.get();

    rev_nn.reserve(pts.size());
    size_t i = 0;
    for(auto& p: pts){
        (*root).add_point(&p);
        rev_nn[i] = PtLoc({rootptr, i});
        i++;
    }
    
    add_vertex(rootptr);
    add_edge(rootptr, rootptr);
    
    cell_heap_vec.push_back({std::move(root), root->radius});
    std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);
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

    auto newcell = std::make_unique<Cell<d, Metric>>(parent->farthest);
    CellPtr newcellptr = newcell.get();
    add_vertex(newcellptr);
    
    rebalance(newcellptr, parent);
    auto neighbors = nbrs(parent);
    for(auto nbr: make_iterator_range(neighbors))
        rebalance(newcellptr, g[nbr]);
    
    debug_log("Finding nbrs of nbrs of " << *(parent->center));
    for(auto& newnbr: nbrs_of_nbrs(parent))
        if(is_close_enough(newcellptr, newnbr))
            add_edge(newcellptr, newnbr);
    add_edge(newcellptr, newcellptr);

    debug_log("Pruning");
    neighbors = nbrs(newcellptr);
    for(auto nbr: make_iterator_range(neighbors))
        prune_nbrs(g[nbr]);
    prune_nbrs(newcellptr);
    
    cell_heap_vec.push_back({std::move(newcell), newcellptr->radius});
    std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);
    
    debug_log("The graph has " << num_vertices(g) << " vertices and " << num_edges(g) << " edges.");
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::rebalance(CellPtr a, CellPtr b){
    vector<Point<d, Metric>*> to_move, to_stay;
    debug_log("PL from " << *(b->center) << " to " << *(a->center));
    size_t i_a = a->size(), i_b = 0;
    for(auto &p: b->points){
        if(a->dist(*p) < b->dist(*p)){
            to_move.push_back(p);
            rev_nn[pt_index(p)] = PtLoc({a, i_a});
            i_a++;
            debug_log("Point: " << *p << ", d(a,p)=" << a->dist(*p) << " and d(b,p)=" << b->dist(*p) << ". Moved");
        }
        else{
            to_stay.push_back(p);
            rev_nn[pt_index(p)] = PtLoc({b, i_b});
            i_b++;
            debug_log("Point: " << *p << ", d(a,p)=" << a->dist(*p) << " and d(b,p)=" << b->dist(*p) << ". Stayed");
        }
    }
    
    b->points = std::move(to_stay);
    
    for(auto &p: to_move)
        a->add_point(p);
    
    if(!to_move.empty())
        b->update_radius();
}

template <std::size_t d, typename Metric>
CellPtrVec<d, Metric> NeighborGraph<d, Metric>::nbrs_of_nbrs(CellPtr c){
    std::unordered_set<CellPtr> output;
    
    auto neighbors = nbrs(c);
    for(auto nbr: make_iterator_range(neighbors)){
        debug_log("Found neighbor of " << *(c->center) << " : " << *(g[nbr]->center));
        auto nbrs_of_nbr = boost::adjacent_vertices(nbr, g);
        for(auto nbr_of_nbr: make_iterator_range(nbrs_of_nbr)){
            debug_log("Found nbr of nbr of " << *(c->center) << " : " << *(g[nbr_of_nbr]->center));
            output.insert(g[nbr_of_nbr]);
        }
    }
    
    output.insert(c);
    
    return vector<CellPtr>(output.begin(), output.end());
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::prune_nbrs(CellPtr c){
    vector<CellPtr> to_delete;
    
    auto neighbors = nbrs(c);
    for(auto nbr: make_iterator_range(neighbors))
        if(!is_close_enough(c, g[nbr])){
            debug_log("Pruning: (" << *(c->center) << ", " << *(g[nbr]->center) << ") as d(c,g[nbr]) = "
                << c->dist(*g[nbr]) << " > " << c->radius << " + " << g[nbr]->radius
                << " + " << max(c->radius, g[nbr]->radius) << " = "
                << c->radius + g[nbr]->radius + max(c->radius, g[nbr]->radius));
            to_delete.push_back(g[nbr]);
        }
    
    for(auto nbr: to_delete)
        boost::remove_edge(vertex[c], vertex[nbr], g);
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
        if(top.second > c->radius){
            // pop top moves highest priority element to back of vector
            std::pop_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);

            // update priority of this element
            cell_heap_vec.back().second = c->radius;

            // push it back into the heap
            std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);
        }
        else{
            debug_log("getting top of heap: " << *(c->center) << " cell radius " << c->radius);
            return c;
        }
    }
    return nullptr;
}

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
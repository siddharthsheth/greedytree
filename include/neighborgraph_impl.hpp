template <std::size_t d, typename Metric>
NeighborGraph<d, Metric>::NeighborGraph(vector<Point<d, Metric>>& P){
    auto root_cell = std::make_unique<Cell<d, Metric>>(P[0]);
    for(auto& p: P)
        (*root_cell).add_point(&p);
    add_vertex(root_cell.get());
    // cell_heap.push({std::move(root_cell), root_cell->radius});
    cell_heap_vec.push_back({std::move(root_cell), root_cell->radius});
    std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::add_vertex(cell_ptr c){
        vertex[c] = boost::add_vertex(c, g); 
    }

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::add_edge(cell_ptr a, cell_ptr b){
    boost::add_edge(vertex[a], vertex[b], g);
}

template <std::size_t d, typename Metric>
bool NeighborGraph<d, Metric>::is_close_enough(const cell_ptr a, const cell_ptr b) const{
    return (*a).dist(*b) <= a->radius + b->radius + max(a->radius, b->radius);
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::add_cell(){
    auto parent = heap_top();
    auto new_cell = std::make_unique<Cell<d, Metric>>(parent->farthest);
    cell_ptr newcellptr = new_cell.get();
    rebalance(newcellptr, parent);
    auto neighbors = boost::adjacent_vertices(vertex[parent], g);
    for(auto nbr: make_iterator_range(neighbors))
        rebalance(newcellptr, g[nbr]);
    
    add_vertex(newcellptr);
    debug_log("Nbrs of nbrs");
    for(auto& newnbr: nbrs_of_nbrs(parent))
        if(is_close_enough(newcellptr, newnbr))
            add_edge(newcellptr, newnbr);
    
    debug_log("Pruning");
    neighbors = boost::adjacent_vertices(vertex[newcellptr], g);
    for(auto nbr: make_iterator_range(neighbors))
        prune_nbrs(g[nbr]);
    prune_nbrs(newcellptr);
    
    // cell_heap.push({std::move(new_cell), newcellptr->radius});
    cell_heap_vec.push_back({std::move(new_cell), newcellptr->radius});
    std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);
    // return newcell;
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::rebalance(
                                cell_ptr a,
                                cell_ptr b
                            ){
    vector<const Point<d, Metric>*> to_move;
    for(auto p: b->points)
        if(a->dist(*p) < b->dist(*p)){
            to_move.push_back(p);
        }
    for(auto p: to_move){
        a->add_point(p);
        b->remove_point(p);
    }
    if(!to_move.empty())
        b->update_radius();
        // update_vertex(b);
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::update_vertex(cell_ptr c){
    c->update_radius();
    g[vertex[c]] = c;
}

template <std::size_t d, typename Metric>
CellPtrVec<d, Metric> NeighborGraph<d, Metric>::nbrs_of_nbrs(cell_ptr c){
    vector<cell_ptr> output;
    auto nbrs = boost::adjacent_vertices(vertex[c], g);
    for(auto nbr: make_iterator_range(nbrs)){
        auto nbrs_of_nbr = boost::adjacent_vertices(nbr, g);
        for(auto nbr_of_nbr: make_iterator_range(nbrs_of_nbr))
            output.push_back(g[nbr_of_nbr]);
    }
    output.push_back(c);
    return output;
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::prune_nbrs(cell_ptr c){
    vector<cell_ptr> to_delete;
    auto nbrs = boost::adjacent_vertices(vertex[c], g);
    for(auto nbr: make_iterator_range(nbrs))
        if(!is_close_enough(c, g[nbr]))
            to_delete.push_back(g[nbr]);
    for(auto nbr: to_delete)
        boost::remove_edge(vertex[c], vertex[nbr], g);
}

template<size_t d, typename Metric>
bool NeighborGraph<d, Metric>::CellCompare::operator()(
                const heap_pair& a,
                const heap_pair& b
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
        cell_ptr c = top.first.get();
        if(top.second > c->radius){
            // pop top
            std::pop_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);

            // update priority
            cell_heap_vec.back().second = c->radius;

            // push it back
            std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);
        }
        else{
            debug_log("getting top of heap: " << *(c->center) << " cell radius " << c->radius);
            return c;
        }
    }
    return nullptr;
}
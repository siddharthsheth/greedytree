template <std::size_t d, typename Metric>
NeighborGraph<d, Metric>::NeighborGraph(vector<pt_ptr> P){
    auto root_cell = std::make_shared<Cell<d, Metric>>(P[0]);
    for(auto p: P)
        (*root_cell).add_point(p);
    add_vertex(root_cell);
    cell_heap.push({root_cell, root_cell->radius});
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
    auto newcell = std::make_shared<Cell<d, Metric>>(parent->farthest);

    rebalance(newcell, parent);
    auto neighbors = boost::adjacent_vertices(vertex[parent], g);
    for(auto nbr: make_iterator_range(neighbors))
        rebalance(newcell, g[nbr]);
    
    add_vertex(newcell);
    // cout << "Nbrs of nbrs" << endl;
    for(auto newnbr: nbrs_of_nbrs(parent))
        if(is_close_enough(newcell, newnbr))
            add_edge(newcell, newnbr);
    
    // cout << "Pruning" << endl;
    neighbors = boost::adjacent_vertices(vertex[newcell], g);
    for(auto nbr: make_iterator_range(neighbors))
        prune_nbrs(g[nbr]);
    prune_nbrs(newcell);
    
    cell_heap.push({newcell, newcell->radius});
    // return newcell;
}

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::rebalance(
                                cell_ptr a,
                                cell_ptr b
                            ){
    unordered_set<std::shared_ptr<const Point<d, Metric>>> to_move;
    for(auto p: b->points)
        if(a->dist(*p) < b->dist(*p)){
            to_move.insert(p);
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
bool NeighborGraph<d, Metric>::CellCmp::operator()(
                const heap_pair a,
                const heap_pair b
    ) const {
    cell_ptr a_c = a.first;
    cell_ptr b_c = b.first;
    double a_r = a.second;
    double b_r = b.second;
    if (a_r != b_r)
        return a_r < b_r;
    if (!(*(a_c->center) == *(b_c->center)))
        return *(a_c->center) < *(b_c->center);
    return a_c->id < b_c->id;  // unique tiebreaker
}

template<size_t d, typename Metric>
CellPtr<d, Metric> NeighborGraph<d, Metric>::heap_top(){
    bool finish = false;
    heap_pair top;
    while(!finish){
        top = cell_heap.top();
        cell_ptr c = top.first;
        double r = top.second;
        if(r > c->radius){
            cell_heap.pop();
            cell_heap.push({c, c->radius});
        }
        else
            finish = true;
        // cout << "getting top of heap: " << r << " cell radius " << c->radius << endl;
    }
    return top.first;
}
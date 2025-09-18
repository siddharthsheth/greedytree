template <std::size_t d, typename Metric>
NeighborGraph<d, Metric>::NeighborGraph(vector<Pt>& pts): root_pt(&pts[0]){
    size_t n = pts.size();

    rev_nn.reserve(n);
    // cells.reserve(n);
    // vertices.reserve(n);
    
    // root_pt = &pts[0];
    auto root = std::make_unique<Cell<d, Metric>>(pts[0]);
    CellPtr rootptr = root.get();

    size_t i = 0;
    for(auto& p: pts){
        root->add_point(&p);
        rev_nn.push_back(PtLoc({rootptr, i}));
        i++;
    }
    
    // add_vertex(rootptr);
    add_edge(rootptr, rootptr);
    
    cell_heap_vec.push_back(HeapPair({std::move(root), root->radius}));
    std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);

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

    auto newcell = std::make_unique<Cell<d, Metric>>(parent->farthest);
    CellPtr newcellptr = newcell.get();
    // add_vertex(newcellptr);
    add_edge(newcellptr, newcellptr);
    
    // affected_nbrs.erase();
    affected_cells.clear();
    // include the new cell in affected_nbrs
    affected_cells.push_back(newcellptr);
    // rebalance(newcellptr, parent);      // self loops => unnecessary
    // auto neighbors = nbrs(parent);
    // for(auto nbr: make_iterator_range(neighbors))
    for(auto& c: parent->nbrs)
        rebalance(newcellptr, c);
    
    debug_log("Finding nbrs of nbrs of " << *(parent->center));
    // for nbrs_of_nbrs, restrict to affected_nbrs
    // for(auto& neighbor: affected_cells)
    //     for(auto newnbr: make_iterator_range(nbrs(neighbor))){
    //         CellPtr nbrptr = g[newnbr];
    //         if(is_close_enough(newcellptr, nbrptr))
    //             add_edge(newcellptr, nbrptr);
    //     }

    unordered_set<CellPtr> relevant_nbrs;
    for(auto& parent_nbr: affected_cells)
        for(auto& nbrptr: parent_nbr->nbrs)
            if(is_close_enough(newcellptr, nbrptr))
                relevant_nbrs.insert(nbrptr);
    
    for(auto& c: relevant_nbrs)
        add_edge(newcellptr, c);
            
    debug_log("Pruning");
    // prune only affected_nbrs
    for(auto c: affected_cells){
        // prune_nbrs(neighbor);
        auto iter = std::partition(c->nbrs.begin(), c->nbrs.end(), 
                [&](const CellPtr nbr){
                    return is_close_enough(c, nbr);
                });
        c->nbrs.erase(iter, c->nbrs.end());
    }
    // neighbors = nbrs(newcellptr);
    // for(auto nbr: make_iterator_range(neighbors))
        // prune_nbrs(g[nbr]);
    // prune_nbrs(newcellptr);             // self loops => unnecessary
    
    cell_heap_vec.push_back(HeapPair({std::move(newcell), newcellptr->radius}));
    std::push_heap(cell_heap_vec.begin(), cell_heap_vec.end(), comparator);
    // cell_heap.push(HeapPair({newcellptr, newcellptr->radius}));
    
    debug_log("The graph has " << num_vertices(g) << " vertices and " << num_edges(g) << " edges.");
}

// template <std::size_t d, typename Metric>
// void NeighborGraph<d, Metric>::rebalance(CellPtr a, CellPtr b){
//     vector<Point<d, Metric>*> to_move, to_stay;
//     debug_log("PL from " << *(b->center) << " to " << *(a->center));
//     size_t i_a = a->size(), i_b = 0;
//     for(auto &p: b->points){
//         if(a->compare_dist(*p) < b->compare_dist(*p)){
//             to_move.push_back(std::move(p));
//             rev_nn[index(p)] = PtLoc({a, i_a});
//             i_a++;
//             debug_log("Point: " << *p << ", d(a,p)=" << a->dist(*p) << " and d(b,p)=" << b->dist(*p) << ". Moved");
//         }
//         else{
//             to_stay.push_back(std::move(p));
//             rev_nn[index(p)] = PtLoc({b, i_b});
//             i_b++;
//             debug_log("Point: " << *p << ", d(a,p)=" << a->dist(*p) << " and d(b,p)=" << b->dist(*p) << ". Stayed");
//         }
//     }
    
//     b->points = std::move(to_stay);
    
//     for(auto &p: to_move)
//         a->add_point(p);
    
//     if(!to_move.empty())
//         b->update_radius();
// }

template <std::size_t d, typename Metric>
void NeighborGraph<d, Metric>::rebalance(CellPtr a, CellPtr b){
    // instead of moving b->points, make points a reference to it
    vector<PtPtr>& points = b->points;
    debug_log("PL from " << *(b->center) << " to " << *(a->center));
    auto b_iter = std::partition(points.begin(), points.end(), [&](const PtPtr p){
        return a->compare_dist(*p) < b->compare_dist(*p);
    });

    
    // if b_iter != points.end(), do the following
    if(b_iter != points.end()){
        // also add b to affected_nbrs
        affected_cells.push_back(b);
        
        size_t i_a = a->size();
        a->points.insert(a->points.end(), points.begin(), b_iter);
        // b->points = std::vector<PtPtr>(b_iter, points.end());
        points.erase(points.begin(), b_iter);
        
        for (auto p_it = a->points.begin()+i_a; p_it != a->points.end(); p_it++) {
            double dist_p = a->dist(**p_it);
            if (dist_p > a->radius) {
                a->radius = dist_p;
                a->farthest = *p_it;
            }
            rev_nn[index(*p_it)] = PtLoc({a, i_a});
            i_a++;
            if(*p_it == b->farthest)
                b->update_radius();
        }
        
        size_t i_b = 0;
        for(auto &p: b->points){
            rev_nn[index(p)] = PtLoc({b, i_b});
            i_b++;
        }
    }
    
    // can be omitted
    // if(points.size() > b->points.size())
    //     b->update_radius();
}

// template <std::size_t d, typename Metric>
// CellPtrVec<d, Metric> NeighborGraph<d, Metric>::nbrs_of_nbrs(CellPtr c){
//     std::unordered_set<CellPtr> output;
    
//     auto neighbors = nbrs(c);
//     for(auto nbr: make_iterator_range(neighbors)){
//         debug_log("Found neighbor of " << *(c->center) << " : " << *(g[nbr]->center));
//         auto nbrs_of_nbr = boost::adjacent_vertices(nbr, g);
//         for(auto nbr_of_nbr: make_iterator_range(nbrs_of_nbr)){
//             debug_log("Found nbr of nbr of " << *(c->center) << " : " << *(g[nbr_of_nbr]->center));
//             output.insert(g[nbr_of_nbr]);
//         }
//     }
    
//     output.insert(c);
    
//     return vector<CellPtr>(output.begin(), output.end());
// }

// template <std::size_t d, typename Metric>
// void NeighborGraph<d, Metric>::prune_nbrs(CellPtr c){
//     vector<CellPtr> to_delete;
    
//     auto neighbors = nbrs(c);
//     for(auto nbr: make_iterator_range(neighbors)){
//         CellPtr nbrptr = g[nbr];
//         if(!is_close_enough(c, nbrptr)){
//             debug_log("Pruning: (" << *(c->center) << ", " << *(nbrptr->center) << ") as d(c,nbrptr) = "
//                 << c->dist(*nbrptr) << " > " << c->radius << " + " << nbrptr->radius
//                 << " + " << max(c->radius, nbrptr->radius) << " = "
//                 << c->radius + nbrptr->radius + max(c->radius, nbrptr->radius));
//             to_delete.push_back(nbrptr);
//         }
//     }
    
//     for(auto nbr: to_delete)
//         // boost::remove_edge(vertex[c], vertex[nbr], g);
//         boost::remove_edge(vertices[index(c->center)], vertices[index(nbr->center)], g);
// }

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
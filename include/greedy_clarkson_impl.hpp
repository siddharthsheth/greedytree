template <std::size_t d, typename Metric>
void clarkson(PtVec<d, Metric>& pts, PtPtrVec<d, Metric>& pred){
    using CellT = Cell<d, Metric>;
    using Pt = Point<d, Metric>;

    pred = vector<const Pt*>(pts.size(), nullptr);

    if (pts.empty())
        return;

    size_t n = pts.size();
    NeighborGraph<d, Metric> G(pts);
    CellT* root_cell = G.heap_top();
    pts = vector<Pt>({*(root_cell->center)});
    
    debug_log("Center of root is at " << *(root_cell->center));
    
    pred[0] = nullptr;
    
    for(auto i = 1; i < n; i++){
        CellT* cell = G.heap_top();
        // size_t far_i = cell->farthest - &pts[0];
        
        debug_log("Center of curr cell is at " << *(cell->center) << " and its farthest point is " << cell->points[cell->farthest]);

        // G.swap_cells(i, far_i);        
        // std::swap(pts[i], pts[far_i]);
        // pts.push_back(*(cell->farthest));
        pred[i] = cell->center;
        
        G.add_cell();
    }
    pts = std::move(G.permutation);
    debug_log("Number of cells created: " << CellT::next_id);
}
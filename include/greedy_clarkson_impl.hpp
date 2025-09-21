// template <std::size_t d, typename Metric>
// void clarkson(PtVec<d, Metric>& pts, PtPtrVec<d, Metric>& pred){
//     using CellT = Cell<d, Metric>;
//     using Pt = Point<d, Metric>;

//     pred = vector<const Pt*>(pts.size(), nullptr);

//     if (pts.empty())
//         return;

//     size_t n = pts.size();
//     NeighborGraph<d, Metric> G(pts);
//     CellT* root_cell = G.heap_top();
//     pts = vector<Pt>({*(root_cell->center)});
    
//     debug_log("Center of root is at " << *(root_cell->center));
    
//     pred[0] = nullptr;
    
//     for(auto i = 1; i < n; i++){
//         CellT* cell = G.heap_top();
//         // size_t far_i = cell->farthest - &pts[0];
        
//         debug_log("Center of curr cell is at " << *(cell->center) << " and its farthest point is " << cell->points[cell->farthest]);

//         // G.swap_cells(i, far_i);        
//         // std::swap(pts[i], pts[far_i]);
//         // pts.push_back(*(cell->farthest));
//         pred[i] = cell->center;
        
//         G.add_cell();
//     }
//     pts = std::move(G.permutation);
//     debug_log("Number of cells created: " << CellT::next_id);
// }

template <std::size_t d, typename Metric>
void clarkson(PtVec<d, Metric>& pts, vector<size_t>& pred){
    using CellT = Cell<d, Metric>;
    using Pt = Point<d, Metric>;

    size_t n = pts.size();
    size_t num_cells_exist = CellT::next_id;
    pred = vector<size_t>(n, -1);

    if (pts.empty())
        return;

    NeighborGraph<d, Metric> G(pts);
    CellT& root_cell = G.cells[0];
    
    debug_log("Center of root is at " << root_cell.center);
    
    for(auto i = 1; i < n; i++){
        size_t cell_i = G.heap_top();
        CellT& cell = G.cells[cell_i];
        // size_t far_i = cell->farthest - &pts[0];
        
        debug_log("Center of curr cell is at " << cell.center << " and its farthest point is " << cell.points[cell.farthest]);

        pred[i] = cell_i;
        
        G.add_cell();
    }
    pts = std::move(G.get_permutation(true));
    debug_log("Number of cells created: " << CellT::next_id - num_cells_exist);

    display_malloc_usage();
    display_phys_footprint();
}
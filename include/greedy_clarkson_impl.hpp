template <std::size_t d, typename Metric>
void clarkson(PtVec<d, Metric>& pts, vector<size_t>& pred){
    using CellT = Cell<d, Metric>;
    using Pt = Point<d, Metric>;

    size_t n = pts.size();
    size_t num_cells_exist = CellT::next_id;

    // initialize pred
    pred = vector<size_t>(n, -1);

    if (pts.empty())
        return;

    // create neighbor graph
    NeighborGraph<d, Metric> G(pts);

    debug_log("Center of root is at " << G.cells[0].center);
    
#ifdef STAT
    stat_log("i, mean, std_dev, 25, 50, 75, max");
#endif
    for(auto i = 1; i < n; i++){
        // get the index of the cell at the top of the cell heap
        size_t cell_i = G.heap_top();
        // set it to be the parent of the ith pt in the permutation
        pred[i] = cell_i;
        // add the next cell to the neighbor graph
        G.add_cell();

        debug_log("Center of parent cell is at " << G.cells[cell_i].center << " and its farthest point is " << G.cells[cell_i].points[G.cells[cell_i].farthest]);

#ifdef STAT
        vector<size_t> nbrs(G.cells.size(), -1);
        for(auto j = 0; j < G.cells.size(); j++){
            nbrs[j] = G.cells[j].nbrs.size();
        }
        auto [mean, std] = mean_std_dev(nbrs);
        std::sort(nbrs.begin(), nbrs.end());
        stat_log(i<<','<<mean<<','<< std <<','<<nbrs[i/4]<<','<<nbrs[i/2]<<','<<nbrs[3*i/4]<<','<<nbrs.back());
#endif
    }
    // extract the greedy permutation from the neighbor graph
    pts = std::move(G.get_permutation(true));

    debug_log("Number of cells created: " << CellT::next_id - num_cells_exist);

#ifdef STAT
    display_malloc_usage();
    display_phys_footprint();
#endif
}
template <std::size_t d, typename Metric>
void clarkson(PtPtrVec<d, Metric>& M,
                PtPtrVec<d, Metric>& gp,
                PtPtrVec<d, Metric>& pred
            ){
    NeighborGraph<d, Metric> G(M);
    CellPtr<d, Metric> root_cell = G.heap_top();
    cout << "Center of root is at " << *(root_cell->center) << endl;
    gp.push_back(root_cell->center);
    pred.push_back(nullptr);
    for(auto i = 1; i < M.size(); i++){
        CellPtr<d, Metric> cell = G.heap_top();
        cout << "Center of curr cell is at " << *(cell->center) << " and its farthest point is " << *(cell->farthest) <<endl;
        gp.push_back(cell->farthest);
        pred.push_back(cell->center);
        G.add_cell();
    }
    cout << "Number of cells created: " << Cell<d, Metric>::next_id << endl;
}
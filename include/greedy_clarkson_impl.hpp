template <std::size_t d, typename Metric>

void clarkson(vector<Point<d, Metric>>& pts,
                vector<const Point<d, Metric>*>& gp,
                vector<const Point<d, Metric>*>& pred
            ){
    using CellT = Cell<d, Metric>;

    if (pts.empty()) {
        gp.clear();
        pred.clear();
        return;
    }

    if (pts.size() == 1) {
        gp.clear();
        pred.clear();
        gp.push_back(&pts[0]);
        pred.push_back(&pts[0]);
        return;
    }
    
    NeighborGraph<d, Metric> G(pts);
    CellT* root_cell = G.heap_top();
    
    debug_log("Center of root is at " << *(root_cell->center));
    
    gp.push_back(root_cell->center);
    pred.push_back(nullptr);
    
    for(auto i = 1; i < pts.size(); i++){
        CellT* cell = G.heap_top();
        
        debug_log("Center of curr cell is at " << *(cell->center) << " and its farthest point is " << *(cell->farthest));
        
        gp.push_back(cell->farthest);
        pred.push_back(cell->center);
        
        G.add_cell();
    }
    debug_log("Number of cells created: " << CellT::next_id);

    // Assert that all output vectors have the same length as input
#include <cassert>
    assert(pts.size() == gp.size() && "Input points and centers must have the same length");
    assert(pts.size() == pred.size() && "Input points and predecessors must have the same length");
}
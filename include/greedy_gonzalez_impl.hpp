template <std::size_t d, typename Metric>
void gonzalez(PtVec<d>& pts, vector<size_t>& pred, Metric metric){

    using Pt = std::array<double, d>;

    pred = vector<size_t>(pts.size(), -1);
    std::vector<double> pred_dist(pts.size());

    if (pts.empty())
        return;

    // initialize the first cell
    for(size_t i = 1; i < pts.size(); i++){
        pred[i] = 0;
        pred_dist[i] = metric.compare_dist(pts[0], pts[i]);
    }
    
    // in each iteration
    for(auto i = 1; i < pts.size(); i++){
        // a. find the farthest point from its pred
        auto max_dist = std::max_element(pred_dist.begin()+i, pred_dist.end());
        size_t far_i = std::distance(pred_dist.begin(), max_dist);
        
        // b. Update the output
        std::swap(pts[i], pts[far_i]);
        std::swap(pred[i], pred[far_i]);
        std::swap(pred_dist[i], pred_dist[far_i]);
        
        // c. for each uninserted point, check if it is closer than current pred
        for(auto j = i+1; j < pts.size(); j++){
            double curr_dist = metric.compare_dist(pts[i], pts[j]);
            if(pred_dist[j] > curr_dist){
                pred[j] = i;
                pred_dist[j] = curr_dist;
            }
        }
    }
}

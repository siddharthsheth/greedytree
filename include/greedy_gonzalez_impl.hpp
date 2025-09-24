template <std::size_t d, typename Metric>
void gonzalez(PtVec<d, Metric>& pts, vector<size_t>& pred){

    using Pt = Point<d, Metric>;

    pred = vector<size_t>(pts.size(), -1);

    if (pts.empty())
        return;

    auto parent_dist = [&](size_t j){
        return pts[j].compare_dist(pts[pred[j]]);
    };

    // initialize the first cell
    for(size_t i = 1; i < pts.size(); i++)
        pred[i] = 0;
    
    // in each iteration
    for(auto i = 1; i < pts.size(); i++){
        // a. find the farthest point from its pred
        double max_dist = parent_dist(i);
        size_t far_i = i;
        for(size_t j = i+1; j < pts.size(); j++){
            double curr_dist = parent_dist(j);
            if(curr_dist > max_dist){
                far_i = j;
                max_dist = curr_dist;
            }
        }
        
        // b. Update the output
        std::swap(pts[i], pts[far_i]);
        std::swap(pred[i], pred[far_i]);
        
        // c. for each uninserted point, check if it is closer than current pred
        for(auto j = i+1; j < pts.size(); j++)
            if(parent_dist(j) > pts[j].compare_dist(pts[i]))
                pred[j] = i;
    }
}

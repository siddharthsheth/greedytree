#include"greedy.hpp"
#include<unordered_map>

using namespace std;

template <std::size_t d, typename Metric>
void gonzalez(vector<Point<d, Metric>>& pts,
                vector<const Point<d, Metric>*>& gp,
                vector<const Point<d, Metric>*>& pred
            ){

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

    using Pt = Point<d, Metric>;
    unordered_map<Pt*, Pt*> rev_nn;
    
    // initialize the first cell
    gp.push_back(&pts[0]);
    pred.push_back(nullptr);

    for(auto& p: pts)
        rev_nn[&p] = &pts[0];
    
    // in each iteration
    for(auto it = pts.begin()+1; it != pts.end(); it++){
        // a. find the farthest point from its rev_nn
        double max_dist = 0;
        Pt *farthest = &(*it), *predec = &(*it);
        for(auto it_j = pts.begin(); it_j != pts.end(); it_j++){
            double candidate_dist = (*it_j).dist(*rev_nn[&(*it_j)]);
            if(candidate_dist > max_dist){
                farthest = &(*it_j);
                predec = rev_nn[&(*it_j)];
                max_dist = candidate_dist;
            }
        }
        
        // b. for each uninserted point, check if it is closer than current rev_nn
        for(auto it_j = pts.begin(); it_j != pts.end(); it_j++)
            if((*it_j).dist(*rev_nn[&(*it_j)]) > (*it_j).dist(*farthest))
                rev_nn[&(*it_j)] = farthest;
        
        // c. Update the output
        gp.push_back(farthest);
        pred.push_back(predec);
    }

    // Assert that all output vectors have the same length as input
#include <cassert>
    assert(pts.size() == gp.size() && "Input points and centers must have the same length");
    assert(pts.size() == pred.size() && "Input points and predecessors must have the same length");
}
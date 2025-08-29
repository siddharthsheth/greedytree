#include"greedy.hpp"
#include<unordered_map>

using namespace std;

template <std::size_t d, typename Metric>
void gonzalez(vector<Point<d, Metric>>& M,
                vector<const Point<d, Metric>*>& gp,
                vector<const Point<d, Metric>*>& pred
            ){
    using Pt = Point<d, Metric>;
    unordered_map<Pt*, Pt*> rev_nn;
    
    // initialize the first cell
    gp.push_back(&M[0]);
    pred.push_back(nullptr);

    for(auto& p: M)
        rev_nn[&p] = &M[0];
    
    // in each iteration
    for(auto it = M.begin(); it != M.end(); it++){
        // a. find the farthest point from its rev_nn
        double max_dist = 0;
        Pt *farthest = &(*it), *predec = &(*it);
        for(auto it_j = M.begin(); it_j != M.end(); it_j++){
            double candidate_dist = (*it_j).dist(*rev_nn[&(*it_j)]);
            if(candidate_dist > max_dist){
                farthest = &(*it_j);
                predec = rev_nn[&(*it_j)];
                max_dist = candidate_dist;
            }
        }
        
        // b. for each uninserted point, check if it is closer than current rev_nn
        for(auto it_j = M.begin(); it_j != M.end(); it_j++)
            if((*it_j).dist(*rev_nn[&(*it_j)]) > (*it_j).dist(*farthest))
                rev_nn[&(*it_j)] = farthest;
        
        // c. Update the output
        gp.push_back(farthest);
        pred.push_back(predec);
    }
}
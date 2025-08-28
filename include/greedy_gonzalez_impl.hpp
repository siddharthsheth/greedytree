#include"greedy.hpp"
#include<map>
#include<functional>

using namespace std;

template <std::size_t d, typename Metric>
void gonzalez(vector<Point<d, Metric>>& M,
                vector<Point<d, Metric>*>& gp,
                vector<Point<d, Metric>*>& pred
            ){
    using Pt = Point<d, Metric>;
    int n = M.size();
    unordered_map<Pt*, Pt*> rev_nn;
    
    // initialize the first cell
    gp.push_back(&M[0]);
    pred.push_back(nullptr);

    for(auto& p: M)
        rev_nn[&p] = &M[0];
    
    // in each iteration
    for(size_t i=0; i < n; i++){
        // a. find the farthest point from its rev_nn
        double max_dist = 0;
        // PtRefWrapper farthest = std::cref(M[i]);
        Pt *farthest = &M[i], *predec = &M[i];
        for(int j=0; j < n; j++){
            double candidate_dist = M[j].dist(*rev_nn[&M[j]]);
            if(candidate_dist > max_dist){
                farthest = &M[j];
                predec = rev_nn[&M[j]];
                max_dist = candidate_dist;
            }
        }
        
        // b. for each uninserted point, check if it is closer than current rev_nn
        for(size_t j=0; j < n; j++)
            if(M[j].dist(*rev_nn[&M[j]]) > M[j].dist(*farthest))
                rev_nn[&M[j]] = farthest;
        
        // c. Update the output
        gp.push_back(farthest);
        pred.push_back(predec);
    }
}
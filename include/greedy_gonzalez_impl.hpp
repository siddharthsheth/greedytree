#include"greedy.hpp"
#include<map>

using namespace std;

template <std::size_t d, typename Metric>
void gonzalez(PtPtrVec<d, Metric>& M,
                PtPtrVec<d, Metric>& gp,
                PtPtrVec<d, Metric>& pred
            ){
    int n = M.size();
    unordered_map<PtPtr<d, Metric>, PtPtr<d, Metric>> rev_nn;
    // initialize the first cell
    gp.push_back(M[0]);
    pred.push_back(nullptr);

    for(auto p: M)
        rev_nn[p] = M[0];
    
    // in each iteration
    for(int i=1; i < n; i++){
        // a. find the farthest point from its rev_nn
        double max_dist = 0;
        PtPtr<d, Metric> farthest = nullptr, predec= nullptr;
        for(int j=0; j < n; j++)
            if(M[j]->dist(*rev_nn[M[j]]) > max_dist){
                farthest = M[j];
                predec = rev_nn[M[j]];
                max_dist = M[j]->dist(*rev_nn[M[j]]);
            }
        // b. for each uninserted point, check if it is closer than current rev_nn
        for(int j=0; j < n; j++)
            if(M[j]->dist(*rev_nn[M[j]]) > M[j]->dist(*farthest))
                rev_nn[M[j]] = farthest;
        // c. Update the output
        gp.push_back(farthest);
        pred.push_back(predec);
    }
}
#include <vector>
#include <stack>
#include <cassert>

#include "point.hpp"
#include "metrics.hpp"
#include "balltree.hpp"

template<size_t d>
using Point = std::array<double, d>;

template<size_t d>
using GTNode = std::tuple<Point<d>, double, size_t>;  // center, radius, num_pts

template<size_t d>
using GTPoints = std::vector<std::pair<Point<d>, size_t>>;

using GTData = std::vector<std::pair<double, size_t>>;

template<size_t d>
inline Point<d>& center(GTNode<d>& g) { return std::get<0>(g); }
template<size_t d>
inline double& node_rad(GTNode<d>& g) { return std::get<1>(g); }
template<size_t d>
inline size_t& num_pts(GTNode<d>& g) { return std::get<2>(g); }

template<size_t d>
vector<size_t> children(std::vector<GTNode<d>>& G, size_t node){
    vector<size_t> output;
    size_t curr = node+1;
    size_t stop = node + num_pts(G[node]);
    while(curr != stop){
        output.push_back(curr);
        curr += num_pts(G[curr]);
    }
    return output;
}

template <size_t d, typename Metric>
void fast_gt(BallTree<d, Metric>* root, std::vector<GTNode<d>>& output) {
    output.clear();
    if (!root) return;

    std::stack<BallTree<d, Metric>*> to_traverse;
    to_traverse.push(root);

    while (!to_traverse.empty()) {
        BallTree<d, Metric>* curr = to_traverse.top();
        to_traverse.pop();

        // Visit current node
        output.push_back({*(curr->center), curr->radius, curr->size});

        // Follow the left chain directly
        while (curr->left) {
            // Save the right child for later traversal
            to_traverse.push(curr->right.get());
            curr = curr->left.get();
        }
    }
}

template <size_t d, typename Metric>
void fast_gt(BallTree<d, Metric>* root, GTPoints<d>& pts, GTData& aux) {
    
    pts.clear();
    aux.clear();
    
    if (!root) return;

    pts.reserve(root->size);
    aux.reserve(2*root->size-1);

    std::stack<BallTree<d, Metric>*> to_traverse;
    to_traverse.push(root);

    while (!to_traverse.empty()) {
        BallTree<d, Metric>* curr = to_traverse.top();
        to_traverse.pop();

        // Visit current node
        pts.push_back({*(curr->center), aux.size()});
        aux.push_back({curr->radius, curr->size});

        // Follow the left chain directly
        while (curr->left) {
            // Save the right child for later traversal
            to_traverse.push(curr->right.get());
            curr = curr->left.get();
            aux.push_back({curr->radius, curr->size});
        }
        aux.push_back({0, 1});
    }
}

using Edge = std::tuple<size_t, double, double, size_t, size_t>;    // nbr_index, distance, b_rad, b_pts, aux_index
using EdgeVec = std::vector<Edge>;

struct EdgeComparator{
    bool operator()(const Edge& u, const Edge& v){
        auto [u_i, u_dist, u_rad, u_pts, u_splits] = u;
        auto [v_i, v_dist, v_rad, v_pts, v_splits] = v;
        return u_rad <= v_rad;
    }
};

using SearchRange = std::pair<size_t, size_t>;          // nbr_index, num_pts
using SearchRangeVec = std::vector<SearchRange>;

template<size_t d, typename Metric>
class ApxRngSearch{
    GTPoints<d>& G;
    GTData& aux;
    Metric metric;

    public:
    ApxRngSearch(GTPoints<d>& G, GTData& aux, Metric& metric):
                G(G), aux(aux), metric(metric){}

    void operator()(Point<d> q, double rad, SearchRangeVec& output, double e=0){
        output.clear();
        size_t i=0, j=0;
        while(i < G.size()){
            auto& [p, p_aux] = G[i];
            j = p_aux;
            double p_dist = metric.dist(p, q);
            size_t curr = i;
            while(curr == i){
                auto& [p_rad, p_pts] = aux[j];
                if(p_dist > rad + p_rad)
                    i += p_pts;
                else if(p_dist <= rad - p_rad || p_rad <= e*rad/2) {
                    output.push_back({i, p_pts});
                    i += p_pts;
                }
                else
                    j++;
            }
        }
    }

    void operator()(Point<d> q, double rad, std::vector<size_t>& output, double e=0){
        output.clear();
        SearchRangeVec ranges;
        (*this)(q, rad, ranges, e);
        for(auto [j, n_j]: ranges)
            for(size_t k = j; k < j + n_j; k++)
                output.push_back(k);
    }

    void operator()(GTPoints<d>& G_A,
                    GTData& aux_a,
                    double query_rad,
                    std::vector<SearchRangeVec>& output,
                    double e=0){
        auto[b_rad, b_pts] = aux[0];
        using Search = std::tuple<size_t, EdgeVec, SearchRangeVec>;
        EdgeVec nbrs({{0, 0, b_rad, b_pts, 0}});
        SearchRangeVec absorbed;
        std::stack<Search> to_process;
        to_process.push({0, nbrs, absorbed});
        
        output = std::vector<SearchRangeVec>(G_A.size());
        while(!to_process.empty()){
            auto [a_i, nbrs, absorbed] = to_process.top();
            to_process.pop();
            auto& [a_ctr, a_aux] = G_A[a_i];
            auto [a_rad, a_pts] = aux_a[a_aux];
            size_t a_splits = a_aux;

            for(auto& [b_i, b_dist, b_rad, b_pts, b_splits] : nbrs){
                auto& [b_ctr, b_aux] = G[b_i];
                b_dist = metric.dist(a_ctr, b_ctr);
            }
                    
            EdgeVec new_nbrs;
            while(!nbrs.empty()){
                while(!nbrs.empty()){
                    auto [b_i, b_dist, b_rad, b_pts, b_splits] = nbrs.back();
                    nbrs.pop_back();
                    if(b_dist > query_rad + a_rad + b_rad)
                        continue;
                    else if(b_dist <= query_rad - a_rad - b_rad)
                        absorbed.push_back({b_i, b_pts});
                    else if(b_rad > a_rad){
                        if(b_rad <= e * query_rad/4)
                            absorbed.push_back({b_i, b_pts});
                        else{
                            b_splits++;
                            std::tie(b_rad, b_pts) = aux[b_splits];
                            size_t b_j = b_i+b_pts;
                            auto& [b_j_ctr, b_j_splits] = G[b_j];
                            auto& [b_j_rad, b_j_pts] = aux[b_j_splits];
                            double b_j_dist = metric.dist(a_ctr, b_j_ctr);
                            if(b_j_dist <= query_rad + a_rad + b_j_rad)
                                nbrs.push_back({b_j,
                                            b_j_dist,
                                            b_j_rad,
                                            b_j_pts,
                                            b_j_splits
                                        });
                            // if(b_dist <= query_rad + a_rad + b_rad)
                            nbrs.push_back({b_i, b_dist, b_rad, b_pts, b_splits});
                        }
                    }
                    else
                        new_nbrs.push_back({b_i, b_dist, b_rad, b_pts, b_splits});
                }
                nbrs = std::move(new_nbrs);
                std::reverse(nbrs.begin(), nbrs.end());
                new_nbrs.clear();

                if(!nbrs.empty()){
                    assert(a_rad > 0);
                    a_splits++;
                    std::tie(a_rad, a_pts) = aux_a[a_splits];
                    // apx_rng_search(a_i + a_pts, nbrs, absorbed, output, e);
                    to_process.push({a_i + a_pts, nbrs, absorbed});
                }
            }

            // finish node a_i
            for(size_t i = a_i; i < a_i + a_pts; i++)
                output[i] = absorbed;
        }
    }

    void operator()(GTPoints<d>& G_A,
                    GTData& aux_a,
                    double query_rad,
                    std::vector<std::vector<size_t>>& output,
                    double e=0){
        output = std::vector<std::vector<size_t>>(G_A.size());
        std::vector<SearchRangeVec> ranges;
        (*this)(G_A, aux_a, query_rad, ranges, e);
        for(size_t i = 0; i < output.size(); i++){
            vector<size_t> points;
            for(auto [j, n_j]: ranges[i])
                for(size_t k = j; k < j + n_j; k++)
                    points.push_back(k);
            output[i] = std::move(points);
        }
    }
};

template<size_t d, typename Metric>
class ApxNNSearch{

    GTPoints<d>& G;
    GTData& aux;
    Metric& metric;

    EdgeComparator edge_compare;

    public:
    ApxNNSearch(GTPoints<d>& G,
            GTData& aux,
            Metric& metric):
            G(G), aux(aux), metric(metric){}

    size_t operator()(Point<d> q, double e=0){
        auto& [a, splits] = G[0];
        auto [rad, pts] = aux[splits];
        
        double nn_dist = metric.dist(a, q);
        size_t nn = 0;
        
        EdgeVec nbrs({{0, nn_dist, rad, pts, splits}});
        std::make_heap(nbrs.begin(), nbrs.end(), edge_compare);
        
        while(!nbrs.empty()) {
            std::pop_heap(nbrs.begin(), nbrs.end(), edge_compare);
            auto [a_i, a_dist, a_rad, a_pts, a_splits] = nbrs.back();
            nbrs.pop_back();
            
            // check pruning condition here
            if(a_dist < nn_dist + a_rad) {
                a_splits++;
                std::tie(a_rad, a_pts) = aux[a_splits];
                
                size_t b_i = a_i+a_pts;
                auto& [b, b_splits] = G[b_i];
                auto& [b_rad, b_pts] = aux[b_splits];
                
                double b_dist = metric.dist(q, b);
                if(b_dist < nn_dist) {
                    nn_dist = b_dist;
                    nn = b_i;
                }
                
                nbrs.push_back({b_i, b_dist, b_rad, b_pts, b_splits});
                std::push_heap(nbrs.begin(), nbrs.end(), edge_compare);
                
                nbrs.push_back({a_i, a_dist, a_rad, a_pts, a_splits});
                std::push_heap(nbrs.begin(), nbrs.end(), edge_compare);
            }
        }
        return nn;
    }

    void operator()(GTPoints<d>& G_A,
                        GTData& aux_a,
                        std::vector<size_t>& output,
                        double e=0
                    ){
        auto[b_rad, b_pts] = aux[0];
        using Search = std::tuple<size_t, EdgeVec>;
        EdgeVec nbrs({{0, 0, b_rad, b_pts, 0}});
        std::make_heap(nbrs.begin(), nbrs.end(), edge_compare);

        std::stack<Search> to_process;
        to_process.push({0, nbrs});
        
        output = std::vector<size_t>(G_A.size());
        while(!to_process.empty()){
            // pop off the next query node from the stack
            auto [a_i, nbrs] = to_process.top();
            to_process.pop();
            auto& [a_ctr, a_aux] = G_A[a_i];
            auto [a_rad, a_pts] = aux_a[a_aux];
            size_t a_splits = a_aux;

            // update its nearest nbr and distances to the nbrs
            double nn_dist = std::numeric_limits<double>::max();
            size_t nn;

            for(auto& [b_i, b_dist, b_rad, b_pts, b_splits] : nbrs) {
                auto& [b_ctr, b_aux] = G[b_i];
                b_dist = metric.dist(a_ctr, b_ctr);
                if(b_dist < nn_dist) {
                    nn_dist = b_dist;
                    nn = b_i;
                }
            }

            // complete the search for the current node
            while(!nbrs.empty()) {
                // check the nbr at the top of the heap
                std::pop_heap(nbrs.begin(), nbrs.end(), edge_compare);
                auto [b_i, b_dist, b_rad, b_pts, b_splits] = nbrs.back();
                
                if(b_rad > a_rad) {
                    // the b node needs to be processed
                    nbrs.pop_back();
                    // check pruning condition here
                    if(b_dist <= nn_dist + 2*a_rad + b_rad) {
                        // cant prune the node, so we split it
                        b_splits++;
                        // get the left child
                        std::tie(b_rad, b_pts) = aux[b_splits];
                        
                        // get the right child
                        size_t b_j = b_i+b_pts;
                        auto& [b_j_ctr, b_j_splits] = G[b_j];
                        auto& [b_j_rad, b_j_pts] = aux[b_j_splits];
                        
                        // update nn_dist with the center of the right child
                        double new_dist = metric.dist(a_ctr, b_j_ctr);
                        if(new_dist < nn_dist) {
                            nn_dist = new_dist;
                            nn = b_j;
                        }
                        
                        // add both edges back to the viable set
                        nbrs.push_back({b_j, new_dist, b_j_rad, b_j_pts, b_j_splits});
                        std::push_heap(nbrs.begin(), nbrs.end(), edge_compare);
                        
                        nbrs.push_back({b_i, b_dist, b_rad, b_pts, b_splits});
                        std::push_heap(nbrs.begin(), nbrs.end(), edge_compare);
                    }
                }
                else {
                    // check finishing condition here
                    if(nn_dist * e >= (3 + 2 * e) * a_rad)
                        break;
                    // else split the query node
                    std::push_heap(nbrs.begin(), nbrs.end(), edge_compare);
                    assert(a_rad > 0);
                    a_splits++;
                    std::tie(a_rad, a_pts) = aux_a[a_splits];
                    // add right child to the stack
                    to_process.push({a_i + a_pts, nbrs});
                }
            }

            // finish node a_i
            for(size_t i = a_i; i < a_i + a_pts; i++)
                output[i] = nn;
        }
        // apx_nn_search(0, nbrs, output, e);
    }

};
#include "balltree.hpp"
#include "point.hpp"
#include <vector>
#include <algorithm>
#include <iterator>

template<size_t d, typename Metric>
struct ViableNode{
    std::array<double, d> center;
    double radius;
    vector<size_t> nbrs;
    bool finished;
};

template<size_t d, typename Metric>
void all_range(vector<typename BallTree<d, Metric>::HeapOrderEntry>& g_a,
            vector<typename BallTree<d, Metric>::HeapOrderEntry>& g_b,
            double query_radius,
            std::vector<std::vector<size_t>>& output){
            // std::unordered_map<std::array<double, d>, std::vector<size_t>>& output){
    
    using NbrPair = std::pair<size_t, double>;

    auto a_it = g_a.begin();
    auto b_it = g_b.begin();

    if(a_it == g_a.end() || b_it == g_b.end())
        return;
    
    double a_r = a_it->radius, b_r = b_it->radius;

    std::vector<double> a_radii, b_radii;
    std::vector<std::array<double, d>> a_centers, b_centers;
    std::vector<std::vector<NbrPair>> a_nbrs, b_nbrs;
    std::vector<bool> finished;

    auto insert_node = [&](bool push_a,
                            std::array<double, d> center,
                            double radius,
                            std::vector<NbrPair> nbrpairs,
                            bool par_fin
                        ){
        if(push_a){
            a_centers.push_back(center);
            a_radii.push_back(radius);
            a_nbrs.push_back(std::move(nbrpairs));
            finished.push_back(par_fin);
        } else {
            b_centers.push_back(center);
            b_radii.push_back(radius);
            b_nbrs.push_back(std::move(nbrpairs));
        }
    };

    double center_dist = (a_it->center).dist(b_it->center);
    a_centers.reserve(g_a.size());
    a_radii.reserve(g_a.size());
    a_nbrs.reserve(g_a.size());
    finished.reserve(g_a.size());
    insert_node(true,
                    a_it->center,
                    a_it->radius,
                    vector<NbrPair>({{0, center_dist}}),
                    false
                );
    
    b_centers.reserve(g_b.size());
    b_radii.reserve(g_b.size());
    b_nbrs.reserve(g_b.size());
    insert_node(false,
                    b_it->center,
                    b_it->radius,
                    vector<NbrPair>({{0, center_dist}}),
                    false
                );
    
    a_it++;
    b_it++;

    auto prune = [&](const size_t i){
        auto iter = std::remove_if(a_nbrs[i].begin(), a_nbrs[i].end(), [&](const NbrPair nbr){
            auto [j, ctr_dist] = nbr;
            if(ctr_dist > query_radius + a_radii[i] + b_radii[j])
                debug_log("prune: Removing edge between " << a_centers[i] << " and " << b_centers[j] << " because " << ctr_dist << ">" << query_radius + a_radii[i] + b_radii[j]);
            else
                debug_log("prune: Keeping edge between " << a_centers[i] << " and " << b_centers[j] << " because " << ctr_dist << "<=" << query_radius + a_radii[i] + b_radii[j]);
            return ctr_dist > query_radius + a_radii[i] + b_radii[j];
        });
        a_nbrs[i].erase(iter, a_nbrs[i].end());
    };

    auto prune_b = [&](const size_t i){
        auto iter = std::remove_if(b_nbrs[i].begin(), b_nbrs[i].end(), [&](const NbrPair nbr){
            auto [j, ctr_dist] = nbr;
            if(ctr_dist > query_radius + a_radii[j] + b_radii[i])
                debug_log("b_prune: Removing edge between " << a_centers[j] << " and " << b_centers[i] << " because " << ctr_dist << ">" << query_radius + a_radii[j] + b_radii[i]);
            else
                debug_log("b_prune: Keeping edge between " << a_centers[j] << " and " << b_centers[i] << " because " << ctr_dist << "<=" << query_radius + a_radii[j] + b_radii[i]);
            return ctr_dist > query_radius + a_radii[j] + b_radii[i];
        });
        b_nbrs[i].erase(iter, b_nbrs[i].end());
    };

    auto finish = [&](const size_t i){
        // if a.nbrs.size() == 0 or rmax <= (self.e / 4) * self.r:
        if(all_of(a_nbrs[i].begin(), a_nbrs[i].end(), [&](const NbrPair nbr){
            auto [j, ctr_dist] = nbr;
            return ctr_dist <= query_radius - a_radii[i] - b_radii[j];
        })){
            debug_log("finish: Finishing node " << a_centers[i]);
            finished[i] = true;
        }
    };

    std::vector<size_t> affected;
    int i = 0;
    while(a_it != g_a.end() || b_it != g_b.end()){
        bool split_a = (a_r >= b_r);
        if(split_a){
            debug_log("split: Iteration " << i++ << " splitting a because " << a_r << ">=" << b_r);
        } else {
            debug_log("split: Iteration " << i++ << " splitting b because " << a_r << "<" << b_r);
        }

        vector<typename BallTree<d, Metric>::HeapOrderEntry>& g = split_a ? g_a : g_b;
        auto it = (split_a ? a_it : b_it);
        double& r = (split_a ? a_r : b_r);
        size_t par_i = it->parent_index;

        std::vector<std::array<double, d>>& centers = (split_a ? a_centers : b_centers);
        std::vector<std::array<double, d>>& other_centers = (split_a ? b_centers : a_centers);
        std::vector<double>& radii = (split_a ? a_radii : b_radii);
        std::vector<std::vector<NbrPair>>& nbrs = (split_a ? a_nbrs : b_nbrs);
        std::vector<std::vector<NbrPair>>& other_nbrs = (split_a ? b_nbrs : a_nbrs);

        vector<NbrPair>& parent_nbrs = nbrs[par_i];
        bool parent_fin = (split_a ? finished[par_i] : false);

        if(!split_a)
            prune_b(par_i);
        
        debug_log("Parent is " << centers[par_i] << " with radius " << radii[par_i]);
        affected.clear();
        size_t new_index = centers.size();

        if (split_a) {
            if (!finished[par_i])
                affected = { par_i, new_index};
        } else {
            // affected.reserve(parent_nbrs.size());
            for(auto [i, par_dist]: parent_nbrs)
                if(!finished[i])
                    affected.push_back(i);
        }
        debug_log("There are " << affected.size() << " affected nodes.");
        debug_log("The parent has " << parent_nbrs.size() << " nbrs.");

        // add new node to viability graph
        std::vector<NbrPair> new_nbrs;
        new_nbrs.reserve(parent_nbrs.size());
        for(auto& [i, par_dist]: parent_nbrs){
            double ctr_dist = (it->center).dist(other_centers[i]);
            new_nbrs.push_back({i, ctr_dist});
            other_nbrs[i].push_back({new_index, ctr_dist});
        }
        insert_node(split_a, it->center, it->radius, new_nbrs, parent_fin);
        // nodes.push_back({it->center, it->radius, parent_nbrs, parent_fin});
        
        // debug_log("Now the parent has " << nodes[it->parent_index].nbrs.size() << " nbrs.");

        // auto& new_node = nodes.back();
        // if(split_a){
        //     debug_log("all_range: Added a node: " <<  new_node.center << " with radius " << new_node.radius << " and " << new_node.nbrs.size() << " nbrs.");
        // } else {
        //     debug_log("all_range: Added b node: " <<  new_node.center << " with radius " << new_node.radius << " and " << new_node.nbrs.size() << " nbrs.");
        // }

        // update radius of parent node
        // nodes[it->parent_index].radius = it->left_radius;
        radii[par_i] = it->left_radius;

        // if split node is b, update its list of neighbors
        // if(!split_a){
        //     prune_b(par_i);
        //     prune_b(new_index);
        // }
        
        // add new node as neighbor of its parents neighbors
        // for(auto& i: nbrs[new_index]){
        //     // debug_log("all_range: Updating " << it->center << " as neighbor of " << other[i].center);
        //     other_nbrs[i].push_back({new_index, centers[new_index].dist(other_centers[i])});
        // }
        
        for(size_t i: affected){
            // absorb();
            prune(i);
            finish(i);
        }

        // update splitting radius
        it++;
        // if(it != g.end()){
        //     if(split_a)
        //         debug_log("Updating r_a to radius of " << nodes[it->parent_index].center << " which is " << nodes[it->parent_index].radius);
        //     else
        //         debug_log("Updating r_b to radius of " << nodes[it->parent_index].center << " which is " << nodes[it->parent_index].radius);
        // }
        r = (it != g.end())? radii[it->parent_index] : -1;
        
        if (split_a) a_it = it;
        else b_it = it;
    }

    // output = std::move(a_nbrs);
    output.clear();
    output.reserve(a_nbrs.size());
    // for(auto& nbrs: a_nbrs)
    //     output.push_back(std::move(nbrs));
        // output[node.center] = std::move(node.nbrs);
    for (auto& nbrs: a_nbrs){
        output.push_back({});
        output.back().reserve(nbrs.size());
        for(auto [i, dis]: nbrs)
            output.back().push_back(i);
    }

};

template<size_t d, typename Metric>
void all_range_naive(vector<std::array<double, d>>& points_a,
            vector<std::array<double, d>>& points_b,
            double query_radius,
            Metric metric,
            unordered_map<std::array<double, d>, vector<std::array<double, d>>>& output){
    output.clear();
    for(auto& p: points_a)
        for(auto& q: points_b)
            if(metric.dist(p, q) <= query_radius)
                output[p].push_back(q);
}
/*
Input is heap order traversal of A, B and radius r
Output is a map from points of A to vectors of balltree pointers of B

A viable node has a center, radius, nbrs, 

Input is a pair of vectors<HeapOrderEntry> a, b
Can store viable graph as vector of viable nodes.
If split node is from a, update affected nodes.
If split node is from b, update affected `a` nodes, but prune `b` nodes separately.
Affected nodes are those nodes in a that have not been finished.


Input is a pair of vector<BallTree*> a, b.
Need to store viable graph as map of points to viable nodes.
If split node is from a, update affected nodes.
If split node is from b, update affected `a` nodes, but prune `b` nodes separately.

If `a` node is finished, it should be removed from the viable graph.
An `a` node in the traversal can be ignored if its parent has been finished (not in the map).
A `b` node in the traversal can be ignored if its parent has no active neighbors (or not in the map).
*/
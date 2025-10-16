#include "balltree.hpp"
#include "point.hpp"
#include <vector>
#include <algorithm>
#include <iterator>

template<size_t d, typename Metric>
struct ViableNode{
    Point<d, Metric> center;
    double radius;
    vector<size_t> nbrs;
    bool finished;
};

template<size_t d, typename Metric>
void all_range(vector<typename BallTree<d, Metric>::HeapOrderEntry>& g_a,
            vector<typename BallTree<d, Metric>::HeapOrderEntry>& g_b,
            double query_radius,
            std::vector<std::vector<size_t>>& output){
            // std::unordered_map<Point<d, Metric>, std::vector<size_t>>& output){
    auto a_it = g_a.begin();
    auto b_it = g_b.begin();

    if(a_it == g_a.end() || b_it == g_b.end())
        return;
    
    double a_r = a_it->radius, b_r = b_it->radius;

    std::vector<ViableNode<d, Metric>> a_nodes, b_nodes;
    a_nodes.reserve(g_a.size());
    b_nodes.reserve(g_b.size());
    a_nodes.push_back({a_it->center, a_it->radius, vector<size_t>({0}), false});
    b_nodes.push_back({b_it->center, b_it->radius, vector<size_t>({0}), false});
    
    // std::cout << "Size of one node in vector is " << sizeof(a_nodes[0]) << " bytes." << std::endl;
    // std::cout << "Center is at " << &(a_nodes[0].center) << std::endl;
    // std::cout << "Center first coord is at " << &(a_nodes[0].center[0]) << std::endl;
    // std::cout << "Center second coord is at " << &(a_nodes[0].center[1]) << std::endl;
    // std::cout << "Radius is at " << &(a_nodes[0].radius) << std::endl;
    // std::cout << "Neighbors are at " << &(a_nodes[0].nbrs) << std::endl;
    // std::cout << "Neighbor entry is at " << &(a_nodes[0].nbrs[0]) << std::endl;
    // std::cout << "Size of one double is " << sizeof(a_r) << " bytes." << std::endl;
    // std::cout << "Bool is at " << &(a_nodes[0].finished) << std::endl;

    a_it++;
    b_it++;

    auto prune = [&](const size_t i){
        ViableNode<d,Metric>& a = a_nodes[i];
        auto iter = std::remove_if(a_nodes[i].nbrs.begin(), a_nodes[i].nbrs.end(), [&](const size_t i){
            ViableNode<d,Metric>& b = b_nodes[i];
            if(a.center.dist(b.center) > query_radius + a.radius + b.radius)
                debug_log("prune: Removing edge between " << a.center << " and " << b.center << " because " << a.center.dist(b.center) << ">" << query_radius + a.radius + b.radius);
            else
                debug_log("prune: Keeping edge between " << a.center << " and " << b.center << " because " << a.center.dist(b.center) << "<=" << query_radius + a.radius + b.radius);
            return a.center.dist(b.center) > query_radius + a.radius + b.radius;
        });
        a_nodes[i].nbrs.erase(iter, a_nodes[i].nbrs.end());
    };

    // auto prune = [&](const size_t i){
    //     ViableNode<d,Metric>& a = a_nodes[i];
    //     auto iter = std::remove_if(a_nodes[i].nbrs.begin(), a_nodes[i].nbrs.end(), [&](const size_t i){
    //         ViableNode<d,Metric>& b = b_nodes[i];
    //         return a.center.dist(b.center) > query_radius + a.radius + b.radius;
    //     });
    //     a_nodes[i].nbrs.erase(iter, a_nodes[i].nbrs.end());
    // };

    auto prune_b = [&](const size_t i){
        ViableNode<d,Metric>& b = b_nodes[i];
        auto iter = std::remove_if(b_nodes[i].nbrs.begin(), b_nodes[i].nbrs.end(), [&](const size_t i){
            ViableNode<d,Metric>& a = a_nodes[i];
            if(a.center.dist(b.center) > query_radius + a.radius + b.radius)
                debug_log("b_prune: Removing edge between " << a.center << " and " << b.center << " because " << a.center.dist(b.center) << ">" << query_radius + a.radius + b.radius);
            else
                debug_log("b_prune: Keeping edge between " << a.center << " and " << b.center << " because " << a.center.dist(b.center) << "<=" << query_radius + a.radius + b.radius);
            return a.center.dist(b.center) > query_radius + a.radius + b.radius;
        });
        b_nodes[i].nbrs.erase(iter, b_nodes[i].nbrs.end());
    };

    auto finish = [&](const size_t i){
        ViableNode<d,Metric>& a = a_nodes[i];
        // if a.nbrs.size() == 0 or rmax <= (self.e / 4) * self.r:
        if(a.nbrs.size() == 0 or a.radius == 0){
            debug_log("finish: Finishing node " << a.center);
            a_nodes[i].finished = true;
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

        std::vector<ViableNode<d, Metric>>& nodes = (split_a ? a_nodes : b_nodes);
        std::vector<ViableNode<d, Metric>>& other = (split_a ? b_nodes : a_nodes);
        vector<typename BallTree<d, Metric>::HeapOrderEntry>& g = split_a ? g_a : g_b;
        auto it = (split_a ? a_it : b_it);
        double& r = (split_a ? a_r : b_r);
        ViableNode<d, Metric>& parent = nodes[it->parent_index];
        
        vector<size_t>& parent_nbrs = parent.nbrs;
        bool& parent_finished = parent.finished;

        debug_log("Parent is " << parent.center << " with radius " << parent.radius);
        affected.clear();

        if (split_a) {
            if (!parent_finished)
                affected = { it->parent_index, nodes.size() };
        } else {
            std::copy(
                parent_nbrs.begin(),
                parent_nbrs.end(),
                std::back_inserter(affected)
            );
        }
        debug_log("There are " << affected.size() << " affected nodes.");
        debug_log("The parent has " << parent_nbrs.size() << " nbrs.");

        // add new node to viability graph
        nodes.push_back({it->center, it->radius, parent_nbrs, parent_finished});
        
        // debug_log("Now the parent has " << nodes[it->parent_index].nbrs.size() << " nbrs.");

        auto& new_node = nodes.back();
        if(split_a){
            debug_log("all_range: Added a node: " <<  new_node.center << " with radius " << new_node.radius << " and " << new_node.nbrs.size() << " nbrs.");
        } else {
            debug_log("all_range: Added b node: " <<  new_node.center << " with radius " << new_node.radius << " and " << new_node.nbrs.size() << " nbrs.");
        }

        // update radius of parent node
        nodes[it->parent_index].radius = it->left_radius;

        // if split node is b, update its list of neighbors
        size_t new_idx = nodes.size()-1;
        if(!split_a){
            prune_b(b_it->parent_index);
            prune_b(new_idx);
        }
        
        // add new node as neighbor of its parents neighbors
        for(auto& i: nodes[new_idx].nbrs){
            debug_log("all_range: Updating " << it->center << " as neighbor of " << other[i].center);
            other[i].nbrs.push_back(new_idx);
        }
        
        for(size_t i: affected){
            // absorb();
            prune(i);
            finish(i);
        }

        // update splitting radius
        it++;
        if(it != g.end()){
            if(split_a)
                debug_log("Updating r_a to radius of " << nodes[it->parent_index].center << " which is " << nodes[it->parent_index].radius);
            else
                debug_log("Updating r_b to radius of " << nodes[it->parent_index].center << " which is " << nodes[it->parent_index].radius);
        }
        r = (it != g.end())? nodes[it->parent_index].radius : -1;
        
        if (split_a) a_it = it;
        else b_it = it;
    }

    output.clear();
    output.reserve(a_nodes.size());
    for(auto& node: a_nodes)
        output.push_back(std::move(node.nbrs));
        // output[node.center] = std::move(node.nbrs);
};

template<size_t d, typename Metric>
void all_range_naive(vector<Point<d, Metric>>& points_a,
            vector<Point<d, Metric>>& points_b,
            double query_radius,
            unordered_map<Point<d,Metric>, vector<Point<d,Metric>>>& output){
    output.clear();
    for(auto& p: points_a)
        for(auto& q: points_b)
            if(p.dist(q) <= query_radius)
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
template<size_t d, typename Metric>
BallTree<d, Metric>::BallTree(PtPtr& p)
    : center(p), radius(0), _size(1), left(nullptr), right(nullptr) {}

template<size_t d, typename Metric>
bool BallTree<d, Metric>::isleaf(){
    return left == nullptr;
}

template<size_t d, typename Metric>
double BallTree<d, Metric>::dist(PtPtr p){
    return center->dist(*p);
}

template<size_t d, typename Metric>
BallHeap<d, Metric> BallTree<d, Metric>::heap(){
    BallHeap ball_heap;

    ball_heap.push(this);
    return ball_heap;
}


template<size_t d, typename Metric>
BallTreeUPtr<d, Metric> greedy_tree(PtVec<d, Metric>& pts){
    // Construct the tree topology
    auto root = _construct_tree(pts);
    // Compute radii for each node
    _compute_radii(root.get());

    return std::move(root);
}

template<size_t d, typename Metric>
BallTreeUPtr<d, Metric> _construct_tree(PtVec<d, Metric>& pts)
{
    using PtPtr = const Point<d, Metric>*;
    using BallTreePtr = BallTree<d, Metric>*;
    
    vector<PtPtr> pred;
    clarkson(pts, pred);
    
    PtPtr root_pt = &pts[0];
    auto root = std::make_unique<BallTree<d, Metric>>(root_pt);
    
    unordered_map<PtPtr, BallTreePtr> leaf;
    leaf[&pts[0]] = root.get();

    for(auto i = 1; i < pts.size(); i++){
        auto node = leaf[pred[i]];
        PtPtr right_pt = &pts[i];
        
        node->left = std::make_unique<BallTree<d, Metric>>(pred[i]);
        node->right = std::make_unique<BallTree<d, Metric>>(right_pt);
        
        leaf[pred[i]] = (node->left).get();
        leaf[right_pt] = (node->right).get();
    }

    return std::move(root);
}

// This method computes 2-approximate radii in linear time.
// Computing exact radius requires finding the point farthest from the center for each node.
template <size_t d, typename Metric>
void _compute_radii(BallTree<d, Metric>* root) {
    using BallTreePtr = BallTree<d, Metric>*;

    std::stack<std::pair<BallTreePtr, bool>> stk;
    stk.push({root, false});
    
    while (!stk.empty()) {
        auto [node, visited] = stk.top();
        stk.pop();
        
        if (!node || node->isleaf())
            continue;
        
        if (visited) {
            // Post-order: process after children
            node->radius = std::max(
                node->left->radius,
                node->dist(node->right->center) + node->right->radius
            );
            node->_size = node->left->_size + node->right->_size;
        }
        else {
            // Mark node for second visit after children
            stk.push({node, true});
            stk.push({(node->right).get(), false});
            stk.push({(node->left).get(), false});
        }
    }
}

template <size_t d, typename Metric>
const Point<d, Metric>* BallTree<d, Metric>::nearest(PtPtr query){
    auto viable = heap();
    const Point<d, Metric>* nearest = nullptr;
    double nn_dist = std::numeric_limits<double>::max();
    
    auto is_viable = [&](BallTree<d, Metric>* node){
        return node->dist(query) - node->radius < nn_dist;
    };
    
    while(!viable.empty()){
        auto top = viable.top();
        double top_dist = top->dist(query);
        if(top_dist < nn_dist){
            nearest = top->center;
            nn_dist = top_dist;
        }
        viable.pop();
        if(top->left && is_viable((top->left).get()))
            viable.push((top->left).get());
        if(top->right && is_viable((top->right).get()))
            viable.push((top->right).get());
    }
    return nearest;
}

template <size_t d, typename Metric>
const Point<d, Metric>* BallTree<d, Metric>::farthest(PtPtr query){
    auto viable = heap();
    const Point<d, Metric>* farthest = nullptr;
    double fn_dist = 0.0;
    
    auto is_viable = [&](BallTree<d, Metric>* node){
        return node->dist(query) + node->radius > fn_dist;
    };
    
    while(!viable.empty()){
        auto top = viable.top();
        double top_dist = top->dist(query);
        if(top_dist > fn_dist){
            farthest = top->center;
            fn_dist = top_dist;
        }
        viable.pop();
        if(top->left && is_viable((top->left).get()))
            viable.push((top->left).get());
        if(top->right && is_viable((top->right).get()))
            viable.push((top->right).get());
    }
    return farthest;
}

template <size_t d, typename Metric>
vector<BallTree<d, Metric>*> BallTree<d, Metric>::range(PtPtr query, double q_radius){
    using BallTreePtr = BallTree<d, Metric>*;

    auto viable = heap();
    vector<BallTreePtr> output;
    
    auto is_viable = [&](BallTreePtr node){
        return node->dist(query) - node->radius <= q_radius;
    };
    
    while(!viable.empty()){
        auto top = viable.top();
        double top_dist = top->dist(query);
        viable.pop();
        
        if(top_dist + top->radius <= q_radius){
            output.push_back(top);
            continue;
        }
        
        if(top->left && is_viable((top->left).get()))
            viable.push((top->left).get());
        
        if(top->right && is_viable((top->right).get()))
            viable.push((top->right).get());
    }
    return output;
}
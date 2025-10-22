template<size_t d, typename Metric>
BallTree<d, Metric>::BallTree(PtPtr& p, Metric metric)
    : center(p), radius(0), size(1), left(nullptr), right(nullptr), metric(metric) {}

template<size_t d, typename Metric>
bool BallTree<d, Metric>::isleaf(){
    return left == nullptr;
}

template<size_t d, typename Metric>
double BallTree<d, Metric>::dist(PtPtr p){
    return metric.dist(*center, *p);
}

template<size_t d, typename Metric>
BallHeap<d, Metric> BallTree<d, Metric>::heap(){
    BallHeap ball_heap;

    ball_heap.push(this);
    return ball_heap;
}

template<size_t d, typename Metric>
void BallTree<d, Metric>::get_traversal(vector<HeapOrderEntry>& output){
    output.clear();
    output.reserve(size);
    output.push_back({*center, radius, 0, 0.0});
    
    std::unordered_map<std::array<double, d>, size_t> index;
    index[*center] = 0;
    
    auto to_traverse = heap();
    while(!to_traverse.empty()){
        auto top = to_traverse.top();
        to_traverse.pop();
        if(top->right){
            index[*(top->right->center)] = output.size();
            output.push_back({
                *(top->right->center),    // center
                top->right->radius,       // radius
                index[*(top->center)],    // parent_index
                top->left->radius         // left_radius
            });

            to_traverse.push((top->left).get());
            to_traverse.push((top->right).get());
        }
    }
}

template<size_t d, typename Metric>
void BallTree<d, Metric>::get_traversal(vector<BallTreePtr>& output){
    output.clear();

    auto to_traverse = heap();
    while(!to_traverse.empty()){
        auto top = to_traverse.top();
        to_traverse.pop();
        output.push_back(top);
        if(!top->leaf()){
            to_traverse.push((top->left).get());
            to_traverse.push((top->right).get());
        }
    }
}

template<size_t d, typename Metric>
BallTreeUPtr<d, Metric> greedy_tree(PtVec<d>& pts, Metric metric){
    // Construct the tree topology
    auto root = construct_tree(pts, metric);
    // Compute radii for each node
    compute_radii(root.get());

    return std::move(root);
}

template<size_t d, typename Metric>
BallTreeUPtr<d, Metric> construct_tree(PtVec<d>& pts, Metric metric)
{
    using PtPtr = const std::array<double, d>*;
    using BallTreePtr = BallTree<d, Metric>*;
    
    vector<size_t> pred;
    clarkson(pts, pred, metric);
    
    PtPtr root_pt = &pts[0];
    auto root = std::make_unique<BallTree<d, Metric>>(root_pt, metric);
    
    // unordered_map<size_t, BallTreePtr> leaf;
    vector<BallTreePtr> leaf(pts.size(), nullptr);
    leaf[0] = root.get();

    for(auto i = 1; i < pts.size(); i++){
        auto node = leaf[pred[i]];
        PtPtr right_pt = &pts[i];
        
        node->left = std::make_unique<BallTree<d, Metric>>(node->center, metric);
        node->right = std::make_unique<BallTree<d, Metric>>(right_pt, metric);
        
        leaf[pred[i]] = (node->left).get();
        leaf[i] = (node->right).get();
    }

    return std::move(root);
}

// This method computes 2-approximate radii in linear time.
// Computing exact radius requires finding the point farthest from the center for each node.
template <size_t d, typename Metric>
void compute_radii(BallTree<d, Metric>* root) {
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
            node->size = node->left->size + node->right->size;
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
const std::array<double, d>* BallTree<d, Metric>::nearest(PtPtr query){
    PtPtr nearest = nullptr;
    double nn_dist = std::numeric_limits<double>::max();
    
    auto is_viable = [&](BallTree<d, Metric>* node){
        return node->dist(query) - node->radius < nn_dist;
    };

    auto update = [&](BallTree<d, Metric>* top){
        double top_dist = top->dist(query);
        if(top_dist < nn_dist){
            nearest = top->center;
            nn_dist = top_dist;
        }
    };
    
    generic_search(update, is_viable);
    return nearest;
}

template <size_t d, typename Metric>
const std::array<double, d>* BallTree<d, Metric>::farthest(PtPtr query){
    PtPtr farthest = nullptr;
    double fn_dist = 0.0;
    
    auto is_viable = [&](BallTreePtr node){
        return node->dist(query) + node->radius > fn_dist;
    };

    auto update = [&](BallTreePtr top){
        double top_dist = top->dist(query);
        if(top_dist > fn_dist){
            farthest = top->center;
            fn_dist = top_dist;
        }
    };
   
    generic_search(update, is_viable);
    return farthest;
}

template <size_t d, typename Metric>
vector<BallTree<d, Metric>*> BallTree<d, Metric>::range(PtPtr query, double q_radius){
    vector<BallTreePtr> output;
    
    auto is_viable = [&](BallTreePtr node){
        double q_dist = node->dist(query);
        return (q_dist + node->radius > q_radius) &&
                    (q_dist - node->radius <= q_radius);
    };
    
    auto update = [&](BallTreePtr top){
        if(top->dist(query) + top->radius <= q_radius)
            output.push_back(top);
    };

    generic_search(update, is_viable);
    return output;
}

template <size_t d, typename Metric>
template<typename Update, typename ViableCondition>
void BallTree<d, Metric>::generic_search(Update update, ViableCondition is_viable){
    auto viable = heap();
    while(!viable.empty()){
        auto top = viable.top();
        update(top);
        viable.pop();
        if(top->left && is_viable((top->left).get()))
            viable.push((top->left).get());
        if(top->right && is_viable((top->right).get()))
            viable.push((top->right).get());
    }
}

template <size_t d, typename Metric>
vector<const std::array<double, d>*> BallTree<d, Metric>::points(){
    deque<BallTree*> to_traverse({this});
    vector<PtPtr> output;
    while(!to_traverse.empty()){
        BallTree* curr = to_traverse[0];
        to_traverse.pop_front();
        if(curr->isleaf())
            output.push_back(curr->center);
        else{
            to_traverse.push_back((curr->left).get());
            to_traverse.push_back((curr->right).get());
        }
    }
    return output;
}
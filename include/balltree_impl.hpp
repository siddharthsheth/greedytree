template<size_t d, typename Metric>
BallTree<d, Metric>::BallTree(PtPtr p)
    : center(p), radius(0), _size(1), left(nullptr), right(nullptr) {}

template<size_t d, typename Metric>
bool BallTree<d, Metric>::isleaf(){
    return left == nullptr;
}

template<size_t d, typename Metric>
double BallTree<d, Metric>::dist(const PtPtr p){
    return center->dist(*p);
}

template<size_t d, typename Metric>
BallHeap<d, Metric> BallTree<d, Metric>::heap(){
    // Define the heap type with decltype
    BallHeap ball_heap;

    ball_heap.push(this->get());
    return ball_heap;
}


template<size_t d, typename Metric>
BallTreeUPtr<d, Metric> greedy_tree(PtPtrVec<d, Metric>& M){
    auto root = _construct_tree(M);
    _compute_radii(root);
    return root;
}

template<size_t d, typename Metric>
BallTreeUPtr<d, Metric> _construct_tree(PtPtrVec<d, Metric>& M)
{
    using PtPtr = const Point<d, Metric>*;
    using BallTreePtr = const BallTree<d, Metric>*;
    
    vector<PtPtr> gp, pred;
    clarkson(M, gp, pred);
    
    auto root = std::make_unique<BallTree<d, Metric>>(gp[0]);
    
    unordered_map<PtPtr, BallTreePtr> leaf;
    leaf[gp[0]] = root;
    
    for(auto i = 1; i < gp.size(); i++){
        auto node = leaf[pred[i]];
        
        node->left = std::make_unique<BallTree<d, Metric>>(pred[i]);
        node->right = std::make_unique<BallTree<d, Metric>>(gp[i]);
        
        leaf[pred[i]] = (node->left).get();
        leaf[gp[i]] = (node->right).get();
    }

    return root;
}

template <size_t d, typename Metric>
void _compute_radii(BallTreeUPtr<d, Metric> root) {
    using BallTreePtr = BallTree<d, Metric>*;

    std::stack<std::pair<BallTreePtr, bool>> stk;
    stk.push({root.get(), false});
    
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
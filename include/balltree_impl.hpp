template<size_t d, typename Metric>
BallTree<d, Metric>::BallTree(pt_ptr p)
    : center(p), radius(0), _size(1), left(nullptr), right(nullptr) {}

template<size_t d, typename Metric>
bool BallTree<d, Metric>::isleaf(){
    return left == nullptr;
}

template<size_t d, typename Metric>
double BallTree<d, Metric>::dist(pt_ptr p){
    return center->dist(*p);
}

template<size_t d, typename Metric>
priority_queue<
        BallTreePtr<d, Metric>,
        vector<BallTreePtr<d, Metric>>,
        typename BallTree<d, Metric>::BallRadiusCompare
> BallTree<d, Metric>::heap(){
    using ball_ptr = std::shared_ptr<BallTree<d, Metric>>;

    // Define the heap type with decltype
    std::priority_queue<
        ball_ptr,
        std::vector<ball_ptr>,
        BallRadiusCompare
    > ball_heap;

    ball_heap.push(std::make_shared<BallTree<d, Metric>>(*this));
    return ball_heap;
}


template<size_t d, typename Metric>
BallTreePtr<d, Metric> greedy_tree(PtPtrVec<d, Metric>& M){
    auto root = _construct_tree(M);
    _compute_radii(root);
    return root;
}

template<size_t d, typename Metric>
BallTreePtr<d, Metric> _construct_tree(PtPtrVec<d, Metric>& M)
{
    using pt_ptr = std::shared_ptr<const Point<d, Metric>>;
    using ball_ptr = std::shared_ptr<BallTree<d, Metric>>;
    
    vector<pt_ptr> gp, pred;
    clarkson(M, gp, pred);
    unordered_map<pt_ptr, ball_ptr> leaf;
    auto root = std::make_shared<BallTree<d, Metric>>(gp[0]);
    leaf[gp[0]] = root;
    for(auto i = 1; i < gp.size(); i++){
        auto node = leaf[pred[i]];
        node->left = std::make_shared<BallTree<d, Metric>>(pred[i]);
        node->right = std::make_shared<BallTree<d, Metric>>(gp[i]);
        leaf[pred[i]] = node->left;
        leaf[gp[i]] = node->right;
    }
    return root;
}

template <size_t d, typename Metric>
void _compute_radii(BallTreePtr<d, Metric> root) {
    using ball_ptr = std::shared_ptr<BallTree<d, Metric>>;

    std::stack<std::pair<ball_ptr, bool>> stk;
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
        } else {
            // Mark node for second visit after children
            stk.push({node, true});
            stk.push({node->right, false});
            stk.push({node->left, false});
        }
    }
}
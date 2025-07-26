template <std::size_t d, typename Metric>
Point<d,Metric>::Point() {
    coords.fill(0.0);
}

// Initializer list constructor
template <std::size_t d, typename Metric>
Point<d,Metric>::Point(std::initializer_list<double> list) {
    if (list.size() != d) {
        throw std::invalid_argument("Initializer list must have exactly d elements.");
    }
    std::copy(list.begin(), list.end(), coords.begin());
}

// Access operators
template <std::size_t d, typename Metric>
double& Point<d,Metric>::operator[](std::size_t i)
    { return coords[i]; }

template <std::size_t d, typename Metric>
const double& Point<d,Metric>::operator[](std::size_t i) const
    { return coords[i]; }

// Comparison
template <std::size_t d, typename Metric>
bool Point<d,Metric>::operator==(const Point& other) const{
    for(auto i = 0; i < d; i++)
        if(coords[i] != other[i])
            return false;
    return true;
}

template <std::size_t d, typename Metric>
bool Point<d,Metric>::operator<(const Point& other) const{
    for(auto i = 0; i < d; i++)
        if(coords[i] >= other[i])
            return false;
    return true;
}

// Addition
template <std::size_t d, typename Metric>
Point<d,Metric> Point<d,Metric>::operator+(const Point& other) const {
    Point result;
    for (std::size_t i = 0; i < d; ++i)
        result[i] = coords[i] + other[i];
    return result;
}

// Subtraction
template <std::size_t d, typename Metric>
Point<d,Metric> Point<d,Metric>::operator-(const Point& other) const {
    Point result;
    for (std::size_t i = 0; i < d; ++i)
        result[i] = coords[i] - other[i];
    return result;
}

// Scalar multiplication
template <std::size_t d, typename Metric>
Point<d,Metric> Point<d,Metric>::operator*(double scalar) const {
    Point result;
    for (std::size_t i = 0; i < d; ++i)
        result[i] = coords[i] * scalar;
    return result;
}

// Norm (delegated to metric)
template <std::size_t d, typename Metric>
double Point<d,Metric>::norm() const {
    return Metric::template norm<d>(*this);
}

// Distance (delegated to metric)
template <std::size_t d, typename Metric>
double Point<d,Metric>::dist(const Point& other) const {
    return Metric::template dist<d>(*this, other);
}

// // Output operator
// template <std::size_t d, typename Metric>
// std::ostream& operator<<(std::ostream& os, const Point<d,Metric>& p) {
//     os << "(";
//     for (std::size_t i = 0; i < d; ++i) {
//         os << p[i];
//         if (i != d - 1) os << ", ";
//     }
//     os << ")";
//     return os;
// }

// template std::ostream& operator<< <2, L1Metric>(std::ostream&, const Point<2, L1Metric>&);

// Helper function to combine hashes (from Boost)
inline void hash_combine(std::size_t& seed, std::size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <size_t d, typename Metric>
size_t std::hash<Point<d, Metric>>::operator()(const Point<d, Metric>& p) const {
    size_t seed = 0;
    for (size_t i = 0; i < d; ++i) {
        size_t h = std::hash<double>{}(p[i]);
        hash_combine(seed, h);
    }
    return seed;
}
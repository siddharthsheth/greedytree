#ifndef METRICS_H
#define METRICS_H

#include <cmath>

// Forward declaration
template <std::size_t d, typename Metric> class Point;

// L2 (Euclidean) norm and distance
struct L2Metric {
    template <std::size_t d>
    static double norm(const Point<d, L2Metric>& p) {
        double sum = 0.0;
        for (std::size_t i = 0; i < d; ++i)
            sum += p[i] * p[i];
        return std::sqrt(sum);
    }

    template <std::size_t d>
    static double dist(const Point<d, L2Metric>& a, const Point<d, L2Metric>& b) {
        double sum = 0.0;
        for (std::size_t i = 0; i < d; ++i) {
            double diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
};

// L1 (Manhattan) norm and distance
struct L1Metric {
    template <std::size_t d>
    static double norm(const Point<d, L1Metric>& p) {
        double sum = 0.0;
        for (std::size_t i = 0; i < d; ++i)
            sum += std::abs(p[i]);
        return sum;
    }

    template <std::size_t d>
    static double dist(const Point<d, L1Metric>& a, const Point<d, L1Metric>& b) {
        double sum = 0.0;
        for (std::size_t i = 0; i < d; ++i)
            sum += std::abs(a[i] - b[i]);
        return sum;
    }
};

#endif // METRICS_H
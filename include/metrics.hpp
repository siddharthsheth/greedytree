/**
 * @file metrics.hpp
 * @author Siddarth Sheth
 * @brief Metric structures for norm and distance calculations in d-dimensional space.
 *
 * Provides L2 (Euclidean) and L1 (Manhattan) metrics for use with Point classes.
 */

#ifndef METRICS_H
#define METRICS_H

#include <cmath>

// Forward declaration of Point class template.
// template <std::size_t d, typename Metric> class Point;

/**
 * @brief L2 (Euclidean) metric for norm and distance calculations.
 *
 * Provides static methods for computing the L2 norm and distance between points.
 */
struct L2Metric {
    /**
     * @brief Compute the L2 norm (Euclidean length) of a point.
     * @tparam d Dimensionality of the point.
     * @param p The point whose norm is to be computed.
     * @return The L2 norm of the point.
     */
    template <std::size_t d>
    static double compare_dist(const std::array<double, d>& a, const std::array<double, d>& b) {
        double sum = 0.0;
        for (std::size_t i = 0; i < d; ++i) {
            double diff = a[i] - b[i];
            sum += diff * diff;
        }
        return sum;
    }

    /**
     * @brief Compute the L2 distance (Euclidean distance) between two points.
     * @tparam d Dimensionality of the points.
     * @param a The first point.
     * @param b The second point.
     * @return The L2 distance between a and b.
     */
    template <std::size_t d>
    static double dist(const std::array<double, d>& a, const std::array<double, d>& b) {
        double sum = 0.0;
        for (std::size_t i = 0; i < d; ++i) {
            double diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
};

/**
 * @brief L1 (Manhattan) metric for norm and distance calculations.
 *
 * Provides static methods for computing the L1 norm and distance between points.
 */
struct L1Metric {
    /**
     * @brief Compute the L1 norm (Manhattan length) of a point.
     * @tparam d Dimensionality of the point.
     * @param p The point whose norm is to be computed.
     * @return The L1 norm of the point.
     */
    template <std::size_t d>
    static double compare_dist(const std::array<double, d>& a, const std::array<double, d>& b) {
        double sum = 0.0;
        for (std::size_t i = 0; i < d; ++i)
            sum += std::abs(a[i] - b[i]);
        return sum;
    }

    /**
     * @brief Compute the L1 distance (Manhattan distance) between two points.
     * @tparam d Dimensionality of the points.
     * @param a The first point.
     * @param b The second point.
     * @return The L1 distance between a and b.
     */
    template <std::size_t d>
    static double dist(const std::array<double, d>& a, const std::array<double, d>& b) {
        double sum = 0.0;
        for (std::size_t i = 0; i < d; ++i)
            sum += std::abs(a[i] - b[i]);
        return sum;
    }
};

#endif // METRICS_H
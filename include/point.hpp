/**
 * @file point.hpp
 * @author Siddarth Sheth
 * @brief Point class template for d-dimensional metric spaces.
 *
 * Defines the Point class and related operators for use in spatial algorithms.
 * Designed for compatibility with mlpack and generic metric-based code.
 */

#ifndef POINT_H
#define POINT_H

#include <array>
#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <functional>  // std::hash
#include <cstddef>     // std::size_t
#include "metrics.hpp"

// Forward declaration for friend operator<<
template <std::size_t d, typename Metric>
std::ostream& operator<<(std::ostream& os, const Point<d, Metric>& p);

/**
 * @brief Represents a point in d-dimensional space with a given metric.
 *
 * @tparam d Dimensionality of the point.
 * @tparam Metric Metric type for norm and distance calculations.
 */
template <std::size_t d, typename Metric>
class Point {
public:
    /**
     * @brief Coordinate array for the point.
     */
    std::array<double, d> coords;

    /**
     * @brief Default constructor. Initializes all coordinates to zero.
     */
    Point();

    /**
     * @brief Construct a point from an initializer list.
     * @param list List of coordinate values.
     */
    Point(std::initializer_list<double> list);

    /**
     * @brief Access coordinate by index (mutable).
     * @param i Index of coordinate.
     * @return Reference to coordinate value.
     */
    double& operator[](std::size_t i);
    /**
     * @brief Access coordinate by index (const).
     * @param i Index of coordinate.
     * @return Const reference to coordinate value.
     */
    const double& operator[](std::size_t i) const;

    /**
     * @brief Equality comparison operator.
     * @param other Point to compare with.
     * @return True if all coordinates are equal.
     */
    bool operator==(const Point& other) const;
    /**
     * @brief Lexicographical less-than comparison.
     * @param other Point to compare with.
     * @return True if this point is less than other.
     */
    bool operator<(const Point& other) const;

    /**
     * @brief Add two points coordinate-wise.
     * @param other Point to add.
     * @return Sum of points.
     */
    Point operator+(const Point& other) const;
    /**
     * @brief Subtract two points coordinate-wise.
     * @param other Point to subtract.
     * @return Difference of points.
     */
    Point operator-(const Point& other) const;
    /**
     * @brief Multiply point by a scalar.
     * @param scalar Scalar value.
     * @return Scaled point.
     */
    Point operator*(double scalar) const;

    /**
     * @brief Compute the norm of the point (delegated to Metric).
     * @return Norm of the point.
     */
    double norm() const;
    /**
     * @brief Compute the distance to another point (delegated to Metric).
     * @param other Point to measure distance to.
     * @return Distance between points.
     */
    double dist(const Point& other) const;
    
    /**
     * @brief Output operator for printing points.
     * @param os Output stream.
     * @param p Point to print.
     * @return Reference to output stream.
     */
    friend std::ostream& operator<< <>(std::ostream& os, const Point<d, Metric>& p);
};

/**
 * @brief Constant origin point (all coordinates zero).
 */
template <std::size_t d, typename Metric>
const Point<d, Metric> origin;

/**
 * @brief Output operator for Point.
 * @tparam d Dimensionality.
 * @tparam Metric Metric type.
 * @param os Output stream.
 * @param p Point to print.
 * @return Reference to output stream.
 */
template <std::size_t d, typename Metric>
std::ostream& operator<<(std::ostream& os, const Point<d,Metric>& p) {
    os << "(";
    for (std::size_t i = 0; i < d; ++i) {
        os << p[i];
        if (i != d - 1) os << ", ";
    }
    os << ")";
    return os;
}

namespace std {
    /**
     * @brief Hash specialization for Point, for use in unordered containers.
     */
    template <std::size_t d, typename Metric>
    struct hash<Point<d, Metric>> {
        std::size_t operator()(const Point<d, Metric>& p) const;
    };
}

#include "point_impl.hpp"

#endif // POINT_H
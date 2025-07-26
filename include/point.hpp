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

// Forward declaration so friend lookup works
template <std::size_t d, typename Metric>
std::ostream& operator<<(std::ostream& os, const Point<d, Metric>& p);

template <std::size_t d, typename Metric>
class Point {
public:
    std::array<double, d> coords;

    // Default constructor
    Point();

    // Initializer list constructor
    Point(std::initializer_list<double> list);

    // Access operators
    double& operator[](std::size_t i);
    const double& operator[](std::size_t i) const;

    // Comparison
    bool operator==(const Point& other) const;
    bool operator<(const Point& other) const;

    // Addition
    Point operator+(const Point& other) const;
    // Subtraction
    Point operator-(const Point& other) const;
    // Scalar multiplication
    Point operator*(double scalar) const;

    // Norm (delegated to metric)
    double norm() const;
    // Distance (delegated to metric)
    double dist(const Point& other) const;
    
    // Output operator
    friend std::ostream& operator<< <>(std::ostream& os, const Point<d, Metric>& p);
};

template <std::size_t d, typename Metric>
const Point<d, Metric> origin;

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
    template <std::size_t d, typename Metric>
    struct hash<Point<d, Metric>> {
        std::size_t operator()(const Point<d, Metric>& p) const;
    };
}

#include "point_impl.hpp"

#endif // POINT_H
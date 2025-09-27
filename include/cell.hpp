/**
 * @file cell.hpp
 * @author Siddarth Sheth
 * @brief Defines the Cell class template for spatial partitioning and clustering.
 *
 * A Cell represents a cluster or region in a metric space, holding a set of points,
 * a center, a radius, and the farthest point from the center. Used in spatial data
 * structures and algorithms such as Ball Trees and greedy clustering.
 */

#ifndef Cell_H
#define Cell_H


#include "point.hpp"
#include <vector>
#include <memory>
#include <cassert>

using namespace std;

/**
 * @brief Represents a cell (cluster) in a d-dimensional metric space.
 *
 * @tparam d The dimensionality of the space.
 * @tparam Metric The metric type used for distance calculations.
 */
template<size_t d, typename Metric>
class Cell {
public:
    /**
     * @brief Alias for a constant Point in d dimensions with the given Metric.
     */
    using Pt = Point<d, Metric>;
    /**
     * @brief Alias for a pointer to a constant Point.
     */
    using PtPtr = Point<d, Metric>*;
    using CellPtr = Cell<d, Metric>*;
    
    /**
     * @brief Static counter for assigning unique IDs to cells.
     */
    static int next_id;
    /**
     * @brief Unique identifier for this cell.
     */
    int id;

    /**
     * @brief Pointer to the center point of the cell.
     */
    // Pt center;
    size_t center;
    /**
     * @brief Radius of the cell (distance from center to farthest point).
     */
    double radius;
    /**
     * @brief Vector of pointers to points contained in the cell.
     */
    // std::vector<Pt> points;
    std::vector<size_t> points;
    /**
     * @brief Pointer to the farthest point from the center in the cell.
     */
    size_t farthest;
    std::vector<size_t> nbrs;
    std::vector<Pt>& global_points;
    // /**
    //  * @brief Default constructor. Initializes an empty cell.
    //  */
    // Cell();
    /**
     * @brief Constructs a cell with a single point (by reference).
     * @param p Reference to the point to initialize the cell with.
     */
    // Cell(Pt p);
    Cell(size_t p_i, vector<Pt>& all_points);
    
    /**
     * @brief Computes the distance from the cell's center to a given point.
     * @param p Reference to the point.
     * @return Distance from center to point.
     */
    double dist(Pt& p) const;
    /**
     * @brief Computes the distance between the centers of two cells.
     * @param c Reference to the other cell.
     * @return Distance between centers.
     */
    double dist(const Cell& c) const;
    double dist(size_t p_i) const;

    double compare_dist(Pt& p) const;
    double compare_dist(const Cell& c) const;
    double compare_dist(size_t p_i) const;

    /**
     * @brief Updates the cell's radius and farthest point.
     */
    void update_radius();
    /**
     * @brief Returns the number of points in the cell.
     * @return Number of points.
     */
    size_t size() const;
    /**
     * @brief Equality operator. Checks if two cells have the same center and points.
     * @param other The cell to compare with.
     * @return True if equal, false otherwise.
     */
    bool operator==(const Cell& other) const;

    // Pt pop_farthest();
};

/**
 * @brief Hash function specialization for Cell, for use in unordered containers.
 * @tparam d Dimensionality.
 * @tparam Metric Metric type.
 */
template<size_t d, typename Metric>
struct std::hash<Cell<d, Metric>> {
    size_t operator()(const Cell<d, Metric>& c) const;
};

/**
 * @brief Type alias for Cell hash function.
 */
template<size_t d, typename Metric>
using CellHash = std::hash<Cell<d, Metric>>;

#include "cell_impl.hpp"

#endif

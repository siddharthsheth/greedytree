/**
 * @file greedy.hpp
 * @author Siddarth Sheth
 * @brief Greedy clustering algorithms (Gonzalez and Clarkson) for metric spaces.
 *
 * Provides function templates for greedy clustering algorithms, including
 * Gonzalez's and Clarkson's algorithms, for use in high-dimensional metric spaces.
 */

#ifndef GREEDY_H
#define GREEDY_H

#include "neighborgraph.hpp"
#include "utils.hpp"
#include <vector>

template <std::size_t d>
using PtVec = std::vector<std::array<double, d>>;

template <std::size_t d>
using PtPtrVec = std::vector<const std::array<double, d>*>;

/**
 * @brief Perform Gonzalez's greedy k-center clustering algorithm.
 *
 * @tparam d Dimensionality of the points.
 * @tparam Metric Metric type for distance calculations.
 * @param M Reference to a vector of points to cluster.
 * @param gp Output vector of pointers to selected cluster centers (greedy points).
 * @param pred Output vector of pointers to the predecessor (nearest center) for each point.
 *
 * This function selects cluster centers greedily to maximize the minimum distance
 * between any point and its nearest center.
 */
// template <std::size_t d, typename Metric>
// void gonzalez(PtVec<d, Metric>& pts, PtPtrVec<d, Metric>& pred);
template <std::size_t d, typename Metric>
void gonzalez(PtVec<d>& pts, vector<size_t>& pred, Metric metric);

/**
 * @brief Perform Clarkson's greedy clustering algorithm.
 *
 * @tparam d Dimensionality of the points.
 * @tparam Metric Metric type for distance calculations.
 * @param M Reference to a vector of points to cluster.
 * @param gp Output vector of pointers to selected cluster centers (greedy points).
 * @param pred Output vector of pointers to the predecessor (nearest center) for each point.
 *
 * This function implements Clarkson's variant of greedy clustering for metric spaces.
 */
// template <std::size_t d, typename Metric>
// void clarkson(PtVec<d, Metric>& pts, PtPtrVec<d, Metric>& pred);
template <std::size_t d, typename Metric>
void clarkson(PtVec<d>& pts, vector<size_t>& pred, Metric metric);

#include "greedy_gonzalez_impl.hpp"
#include "greedy_clarkson_impl.hpp"

#endif
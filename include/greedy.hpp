#ifndef GREEDY_H
#define GREEDY_H

#include "neighborgraph.hpp"
#include <vector>

using namespace std;

template <std::size_t d, typename Metric>
using Pt = Point<d, Metric>;

template <std::size_t d, typename Metric>
using PtRefVec = std::vector<std::reference_wrapper<const Point<d, Metric>>>;

template<size_t d, typename Metric>
using PtPtr = std::shared_ptr<const Point<d, Metric>>;

template <std::size_t d, typename Metric>
void gonzalez(vector<Point<d, Metric>>& M,
                vector<const Point<d, Metric>*>& gp,
                vector<const Point<d, Metric>*>& pred
            );

template <std::size_t d, typename Metric>
void clarkson(vector<Point<d, Metric>>& M,
                vector<const Point<d, Metric>*>& gp,
                vector<const Point<d, Metric>*>& pred
            );

#include "greedy_gonzalez_impl.hpp"
#include "greedy_clarkson_impl.hpp"

#endif
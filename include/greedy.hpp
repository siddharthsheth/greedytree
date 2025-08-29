#ifndef GREEDY_H
#define GREEDY_H

#include "neighborgraph.hpp"
#include <vector>

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
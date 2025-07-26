#ifndef GREEDY_H
#define GREEDY_H

#include "neighborgraph.hpp"
#include <vector>

using namespace std;

template <std::size_t d, typename Metric>
using PtPtrVec = std::vector<std::shared_ptr<const Point<d, Metric>>>;

template<size_t d, typename Metric>
using PtPtr = std::shared_ptr<const Point<d, Metric>>;

template <std::size_t d, typename Metric>
void gonzalez(PtPtrVec<d, Metric>& M,
                PtPtrVec<d, Metric>& gp,
                PtPtrVec<d, Metric>& pred
            );

template <std::size_t d, typename Metric>
void clarkson(PtPtrVec<d, Metric>& M,
                PtPtrVec<d, Metric>& gp,
                PtPtrVec<d, Metric>& pred
            );

#include "greedy_gonzalez_impl.hpp"
#include "greedy_clarkson_impl.hpp"

#endif
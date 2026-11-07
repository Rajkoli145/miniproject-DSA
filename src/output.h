#pragma once

#include "graph.h"
#include "tarjan.h"
#include <iosfwd>
#include <vector>

namespace depgraph {

// Raj's output: Adjacency list, SCCs, cyclic groups, self-loops, componentOf.
void printAnalysis(std::ostream& output, const ModuleGraph& modules, const SCCResult& result);

// Pranav's output: condensed edges between SCC groups.
void printCondensedDependencies(std::ostream& output,
                                const Graph& dag,
                                const std::vector<std::vector<int>>& groups,
                                const std::vector<std::string>& names);

// Pranav's output: final dependency-first order of groups.
void printDependencyOrder(std::ostream& output,
                          const std::vector<int>& order,
                          const std::vector<std::vector<int>>& groups,
                          const std::vector<std::string>& names);

}  // namespace depgraph

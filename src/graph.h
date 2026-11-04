#pragma once

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

namespace depgraph {

using Graph = std::vector<std::vector<int>>;

struct ModuleGraph {
    std::vector<std::string> names;
    Graph graph;  // graph[u] contains the modules u depends on.
};

ModuleGraph buildGraph(
    const std::vector<std::string>& names,
    const std::vector<std::pair<std::string, std::string>>& dependencies);

// Format: module count, module names, edge count, then source/target pairs.
// Names are nonempty whitespace-free tokens. Throws invalid_argument on bad input.
ModuleGraph readGraph(std::istream& input);

}  // namespace depgraph

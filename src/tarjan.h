#pragma once

#include "graph.h"

namespace depgraph {

struct SCCResult {
    std::vector<std::vector<int>> groups;
    std::vector<int> componentOf;
    std::vector<bool> cyclic;    // Indexed by component ID.
    std::vector<bool> selfLoop;  // Indexed by module ID.
};

// Accepts an adjacency list directly, independently of console input.
// Invalid vertex IDs throw invalid_argument. All traversal state is per call.
SCCResult findSCCs(const Graph& graph);

}  // namespace depgraph

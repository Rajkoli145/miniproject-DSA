#include "topological_sort.h"

#include <algorithm>
#include <queue>

namespace depgraph {

std::vector<int> dependencyFirstOrder(const Graph& dag) {
    int C = static_cast<int>(dag.size());
    if (C == 0) return {};

    // Step 1: Compute indegree for each group.
    // In edge convention a -> b (a depends on b), b receives an incoming edge.
    std::vector<int> indegree(C, 0);
    for (int a = 0; a < C; ++a) {
        for (int b : dag[a]) {
            ++indegree[b];
        }
    }

    // Step 2: Enqueue all groups with indegree == 0.
    std::queue<int> q;
    for (int i = 0; i < C; ++i) {
        if (indegree[i] == 0) {
            q.push(i);
        }
    }

    // Step 3: BFS processing
    std::vector<int> order;
    order.reserve(C);

    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        order.push_back(curr);

        for (int neighbor : dag[curr]) {
            if (--indegree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }

    // Step 4: Check if all C groups were processed.
    // A correctly condensed SCC graph is always acyclic.
    if (static_cast<int>(order.size()) != C) {
        return {};  // Cycle / inconsistency detected
    }

    // Step 5: Reverse to get dependency-first order.
    // Kahn's produces dependents first (no incoming edges).
    // Reversing puts dependencies first (ready to build).
    std::reverse(order.begin(), order.end());

    return order;
}

}  // namespace depgraph

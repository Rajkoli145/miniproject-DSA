#include "condensation.h"

namespace depgraph {

Graph buildCondensedGraph(const Graph& graph,
                          const std::vector<int>& componentOf,
                          int C) {
    Graph dag(C);
    int V = static_cast<int>(graph.size());

    for (int u = 0; u < V; ++u) {
        int a = componentOf[u];  // group ID of source module u

        for (int v : graph[u]) {
            int b = componentOf[v];  // group ID of target module v

            // Keep cross-group edges; skip intra-group edges
            if (a != b) {
                dag[a].push_back(b);
            }
        }
    }

    return dag;
}

}  // namespace depgraph

#pragma once

#include "graph.h"

#include <vector>

namespace depgraph {

// ──────────────────────────────────────────────────────────────
// buildCondensedGraph
//   Collapses the original module-level graph into a DAG of
//   SCC groups.
//
//   Parameters:
//     graph       – original module adjacency list (graph[u] contains
//                   modules u depends on)
//     componentOf – componentOf[u] is the SCC group ID for module u
//     C           – total number of SCC groups (IDs 0 to C-1)
//
//   Returns:
//     dag – group-level adjacency list (dag[a] contains groups
//           that group a depends on).
//
//   For every original edge u -> v:
//     a = componentOf[u], b = componentOf[v]
//     if a != b: add edge a -> b in dag
//     if a == b: skip (edge remains inside the same SCC group)
//
//   Duplicate cross-group edges are intentionally KEPT so that
//   indegree counts and decrements balance during Kahn's sort.
//
//   Time  : O(V + E)
//   Space : O(C + Ec)
// ──────────────────────────────────────────────────────────────
Graph buildCondensedGraph(const Graph& graph,
                          const std::vector<int>& componentOf,
                          int C);

}  // namespace depgraph

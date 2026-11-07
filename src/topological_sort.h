#pragma once

#include "graph.h"

#include <vector>

namespace depgraph {

// ──────────────────────────────────────────────────────────────
// dependencyFirstOrder  (Kahn's Algorithm + Reverse)
//
//   1. Compute indegrees of all groups in the condensed DAG.
//   2. Enqueue all groups with indegree == 0.
//   3. BFS: dequeue a group, record it, decrement neighbors'
//      indegrees; enqueue any neighbor when its indegree hits 0.
//   4. If fewer than C groups processed, return empty (cycle detected).
//   5. Reverse the order to get dependency-first output.
//
//   Returns:
//     Ordered vector of group IDs, dependencies before dependents.
//     Returns empty vector if internal inconsistency/cycle detected.
//
//   Time  : O(C + Ec)
//   Space : O(C)
// ──────────────────────────────────────────────────────────────
std::vector<int> dependencyFirstOrder(const Graph& dag);

}  // namespace depgraph

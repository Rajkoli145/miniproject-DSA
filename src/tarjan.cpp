#include "tarjan.h"

#include <algorithm>
#include <climits>
#include <functional>
#include <stdexcept>

namespace depgraph {

SCCResult findSCCs(const Graph& graph) {
    if (graph.size() > INT_MAX) throw std::invalid_argument("Too many vertices.");
    const int n = static_cast<int>(graph.size());
    SCCResult result;
    result.componentOf.assign(n, -1);
    result.selfLoop.assign(n, false);
    for (int u = 0; u < n; ++u) {
        for (int v : graph[u]) {
            if (v < 0 || v >= n) throw std::invalid_argument("Invalid dependency vertex ID.");
            if (u == v) result.selfLoop[u] = true;
        }
    }

    std::vector<int> index(n, -1), low(n), stack;
    std::vector<bool> onStack(n, false);
    int nextIndex = 0;
    std::function<void(int)> visit = [&](int u) {
        index[u] = low[u] = nextIndex++;
        stack.push_back(u);
        onStack[u] = true;
        for (int v : graph[u]) {
            if (index[v] == -1) {
                visit(v);
                low[u] = std::min(low[u], low[v]);
            } else if (onStack[v]) {
                low[u] = std::min(low[u], index[v]);
            }
        }
        if (low[u] == index[u]) {
            const int component = static_cast<int>(result.groups.size());
            std::vector<int> group;
            int v;
            do {
                v = stack.back();
                stack.pop_back();
                onStack[v] = false;
                result.componentOf[v] = component;
                group.push_back(v);
            } while (v != u);
            const bool cyclic = group.size() > 1 || result.selfLoop[group.front()];
            result.groups.push_back(std::move(group));
            result.cyclic.push_back(cyclic);
        }
    };
    for (int u = 0; u < n; ++u) {
        if (index[u] == -1) visit(u);
    }
    return result;
}

}  // namespace depgraph

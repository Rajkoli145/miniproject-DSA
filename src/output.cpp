#include "output.h"

#include <ostream>

namespace depgraph {

static void printGroup(std::ostream& out,
                       const std::vector<int>& group,
                       const std::vector<std::string>& names) {
    out << '{';
    for (std::size_t i = 0; i < group.size(); ++i) {
        if (i) out << ", ";
        out << names[group[i]];
    }
    out << '}';
}

void printAnalysis(std::ostream& out, const ModuleGraph& modules, const SCCResult& result) {
    auto printGroupLocal = [&](const std::vector<int>& group) {
        printGroup(out, group, modules.names);
    };
    if (modules.names.empty()) out << "No modules.\n";
    out << "Adjacency list (module -> dependencies):\n";
    for (std::size_t u = 0; u < modules.names.size(); ++u) {
        out << u << " " << modules.names[u] << " -> ";
        printGroupLocal(modules.graph[u]);
        out << '\n';
    }
    out << "Strongly connected components:\n";
    for (std::size_t c = 0; c < result.groups.size(); ++c) {
        out << c << ": ";
        printGroupLocal(result.groups[c]);
        out << (result.cyclic[c] ? " (cyclic)\n" : " (not cyclic)\n");
    }
    out << "Cyclic groups:\n";
    bool any = false;
    for (std::size_t c = 0; c < result.groups.size(); ++c) {
        if (result.cyclic[c]) {
            printGroupLocal(result.groups[c]);
            out << '\n';
            any = true;
        }
    }
    if (!any) out << "None\n";
    out << "Self-loops:\n";
    any = false;
    for (std::size_t u = 0; u < result.selfLoop.size(); ++u) {
        if (result.selfLoop[u]) {
            out << modules.names[u] << '\n';
            any = true;
        }
    }
    if (!any) out << "None\n";
    out << "componentOf = [";
    for (std::size_t u = 0; u < result.componentOf.size(); ++u) {
        if (u) out << ", ";
        out << result.componentOf[u];
    }
    out << "]\n";
}

void printCondensedDependencies(std::ostream& out,
                                const Graph& dag,
                                const std::vector<std::vector<int>>& groups,
                                const std::vector<std::string>& names) {
    out << "Condensed dependencies:\n";
    bool any = false;
    int C = static_cast<int>(dag.size());
    for (int a = 0; a < C; ++a) {
        for (int b : dag[a]) {
            printGroup(out, groups[a], names);
            out << " -> ";
            printGroup(out, groups[b], names);
            out << '\n';
            any = true;
        }
    }
    if (!any) out << "None\n";
}

void printDependencyOrder(std::ostream& out,
                          const std::vector<int>& order,
                          const std::vector<std::vector<int>>& groups,
                          const std::vector<std::string>& names) {
    out << "Dependency-first group order:\n";
    if (order.empty()) {
        out << "None\n";
        return;
    }
    for (std::size_t i = 0; i < order.size(); ++i) {
        if (i) out << ", ";
        printGroup(out, groups[order[i]], names);
    }
    out << '\n';
}

}  // namespace depgraph

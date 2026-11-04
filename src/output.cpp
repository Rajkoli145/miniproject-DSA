#include "output.h"

#include <ostream>

namespace depgraph {

void printAnalysis(std::ostream& out, const ModuleGraph& modules, const SCCResult& result) {
    auto printGroup = [&](const std::vector<int>& group) {
        out << '{';
        for (std::size_t i = 0; i < group.size(); ++i) {
            if (i) out << ", ";
            out << modules.names[group[i]];
        }
        out << '}';
    };
    if (modules.names.empty()) out << "No modules.\n";
    out << "Adjacency list (module -> dependencies):\n";
    for (std::size_t u = 0; u < modules.names.size(); ++u) {
        out << u << " " << modules.names[u] << " -> ";
        printGroup(modules.graph[u]);
        out << '\n';
    }
    out << "Strongly connected components:\n";
    for (std::size_t c = 0; c < result.groups.size(); ++c) {
        out << c << ": ";
        printGroup(result.groups[c]);
        out << (result.cyclic[c] ? " (cyclic)\n" : " (not cyclic)\n");
    }
    out << "Cyclic groups:\n";
    bool any = false;
    for (std::size_t c = 0; c < result.groups.size(); ++c) {
        if (result.cyclic[c]) {
            printGroup(result.groups[c]);
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

}  // namespace depgraph

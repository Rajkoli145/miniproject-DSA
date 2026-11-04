#include "graph.h"
#include "output.h"
#include "tarjan.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace depgraph;
using Partition = std::set<std::set<int>>;

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void rejects(const std::function<void()>& action) {
    try { action(); }
    catch (const std::invalid_argument&) { return; }
    throw std::runtime_error("Expected invalid_argument.");
}

void checkInvariants(const Graph& graph, const SCCResult& result) {
    require(result.componentOf.size() == graph.size(), "componentOf size");
    require(result.selfLoop.size() == graph.size(), "selfLoop size");
    require(result.cyclic.size() == result.groups.size(), "cyclic size");
    std::vector<int> seen(graph.size(), 0);
    for (std::size_t c = 0; c < result.groups.size(); ++c) {
        const auto& group = result.groups[c];
        require(!group.empty(), "Empty SCC");
        for (int u : group) {
            require(u >= 0 && static_cast<std::size_t>(u) < graph.size(), "Bad member ID");
            ++seen[u];
            require(result.componentOf[u] == static_cast<int>(c), "Wrong component mapping");
        }
        const int u = group.front();
        const bool loop = std::find(graph[u].begin(), graph[u].end(), u) != graph[u].end();
        require(result.cyclic[c] == (group.size() > 1 || loop), "Wrong cycle classification");
    }
    for (std::size_t u = 0; u < graph.size(); ++u) {
        require(seen[u] == 1, "Vertex must appear exactly once");
        const bool loop = std::find(graph[u].begin(), graph[u].end(), static_cast<int>(u)) != graph[u].end();
        require(result.selfLoop[u] == loop, "Wrong self-loop flag");
    }
}

void check(const Graph& graph, const Partition& expected) {
    const auto result = findSCCs(graph);
    checkInvariants(graph, result);
    Partition actual;
    for (const auto& group : result.groups) actual.emplace(group.begin(), group.end());
    require(actual == expected, "Wrong SCC partition");
}

// An independent reachability oracle checks every directed graph of up to four
// vertices, including self-loops. Two vertices share an SCC iff mutually reachable.
void exhaustiveCheck() {
    for (int n = 0; n <= 4; ++n) {
        const unsigned limit = 1u << (n * n);
        for (unsigned mask = 0; mask < limit; ++mask) {
            Graph graph(n);
            std::vector<std::vector<bool>> reach(n, std::vector<bool>(n, false));
            for (int u = 0; u < n; ++u) {
                reach[u][u] = true;
                for (int v = 0; v < n; ++v) {
                    if (mask & (1u << (u * n + v))) {
                        graph[u].push_back(v);
                        reach[u][v] = true;
                    }
                }
            }
            for (int k = 0; k < n; ++k)
                for (int u = 0; u < n; ++u)
                    for (int v = 0; v < n; ++v)
                        reach[u][v] = reach[u][v] || (reach[u][k] && reach[k][v]);
            const auto result = findSCCs(graph);
            checkInvariants(graph, result);
            for (int u = 0; u < n; ++u)
                for (int v = 0; v < n; ++v)
                    require((result.componentOf[u] == result.componentOf[v]) ==
                            (reach[u][v] && reach[v][u]), "Reachability oracle mismatch");
        }
    }
}

int main() {
    try {
        check({}, {});
        check({{}}, {{0}});
        check({{1}, {2}, {}}, {{0}, {1}, {2}});
        check({{1}, {2}, {0}}, {{0, 1, 2}});
        check({{0}}, {{0}});
        check({{1}, {0}, {3}, {2}, {}}, {{0, 1}, {2, 3}, {4}});
        check({{1, 1}, {}}, {{0}, {1}});
        check({{1}, {0, 2}, {}}, {{0, 1}, {2}});
        check({{1}, {2}, {1, 3}, {}}, {{0}, {1, 2}, {3}});
        // Edges to an already completed SCC must not lower low-link values.
        check({{}, {0}, {1, 0}}, {{0}, {1}, {2}});
        check({{1}, {0, 2}, {3}, {2}}, {{0, 1}, {2, 3}});
        rejects([] { findSCCs({{1}}); });
        rejects([] { findSCCs({{-1}}); });

        const auto built = buildGraph({"A", "B", "Isolated"}, {{"A", "B"}, {"A", "B"}, {"B", "B"}});
        require(built.names == std::vector<std::string>({"A", "B", "Isolated"}), "Name IDs changed");
        require(built.graph == Graph({{1, 1}, {1}, {}}), "Edges lost or deduplicated");
        rejects([] { buildGraph({"A", "A"}, {}); });
        rejects([] { buildGraph({""}, {}); });
        rejects([] { buildGraph({"A B"}, {}); });
        rejects([] { buildGraph({"A"}, {{"X", "A"}}); });
        rejects([] { buildGraph({"A"}, {{"A", "X"}}); });

        std::istringstream input("4 App Auth User Database 4 App Auth Auth User User Auth User Database");
        const auto modules = readGraph(input);
        require(modules.graph == Graph({{1}, {2}, {1, 3}, {}}), "Sample parsing");
        const auto result = findSCCs(modules.graph);
        std::ostringstream output;
        printAnalysis(output, modules, result);
        require(output.str().find("{User, Auth} (cyclic)") != std::string::npos, "Cycle output missing");
        require(output.str().find("componentOf = [2, 1, 1, 0]") != std::string::npos, "Handoff output missing");
        for (const std::string invalid : {"", "-1", "1.5", "9999999999999", "1", "1 A", "0 -1", "1 A 1 A", "0 0 extra", "2 A A 0", "1 A 1 A X", "1 A 1 X A"}) {
            rejects([&] { std::istringstream bad(invalid); readGraph(bad); });
        }
        std::istringstream empty("0 0");
        const auto noModules = readGraph(empty);
        std::ostringstream emptyOutput;
        printAnalysis(emptyOutput, noModules, findSCCs(noModules.graph));
        require(emptyOutput.str().find("No modules.") != std::string::npos, "Empty output missing");
        exhaustiveCheck();
        std::cout << "All SCC, graph validation, parser, and output checks passed.\n"
                     "Verified 66,067 directed graphs against a reachability oracle.\n";
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}

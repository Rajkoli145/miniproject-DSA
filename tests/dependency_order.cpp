#include "condensation.h"
#include "graph.h"
#include "tarjan.h"
#include "topological_sort.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace {

int testsPassed = 0;
int totalTests  = 0;

// Verifies the two critical invariants:
// 1. All C groups appear exactly once.
// 2. For every condensed edge a -> b (a depends on b), b occurs BEFORE a.
bool verifyOrder(const depgraph::Graph& dag, const std::vector<int>& order) {
    int C = static_cast<int>(dag.size());
    if (static_cast<int>(order.size()) != C) return false;

    // Check all group IDs 0 to C-1 appear exactly once
    std::set<int> uniqueGroups(order.begin(), order.end());
    if (static_cast<int>(uniqueGroups.size()) != C) return false;
    for (int i = 0; i < C; ++i) {
        if (uniqueGroups.count(i) == 0) return false;
    }

    // Build position map: position[groupId] = index in order
    std::vector<int> position(C);
    for (int i = 0; i < C; ++i) {
        position[order[i]] = i;
    }

    // For every edge a -> b (a depends on b), b must precede a in dependency-first order
    for (int a = 0; a < C; ++a) {
        for (int b : dag[a]) {
            if (position[b] >= position[a]) {
                return false;
            }
        }
    }

    return true;
}

// ────────────────────────────────────────────────────────────
// Case 0: Guide Sample (App, Auth, User, Database)
// ────────────────────────────────────────────────────────────
void testGuideSample() {
    ++totalTests;
    std::cout << "Test 0: Guide Sample (App, Auth, User, Database)... ";

    // IDs: App=0, Auth=1, User=2, Database=3
    depgraph::Graph graph = {{1}, {2}, {1, 3}, {}};
    std::vector<int> componentOf = {0, 1, 1, 2};
    int C = 3;

    depgraph::Graph dag = depgraph::buildCondensedGraph(graph, componentOf, C);
    std::vector<int> order = depgraph::dependencyFirstOrder(dag);

    // Expected DAG: Group 0 -> [1], Group 1 -> [2], Group 2 -> []
    assert(dag.size() == 3);
    assert((dag[0] == std::vector<int>{1}));
    assert((dag[1] == std::vector<int>{2}));
    assert(dag[2].empty());

    // Expected dependency-first order: [2, 1, 0] ({Database}, {Auth, User}, {App})
    assert((order == std::vector<int>{2, 1, 0}));
    assert(verifyOrder(dag, order));

    std::cout << "PASSED\n";
    ++testsPassed;
}

// ────────────────────────────────────────────────────────────
// Case 1: Empty graph
//   "Return an empty ordering without crashing."
// ────────────────────────────────────────────────────────────
void testEmptyGraph() {
    ++totalTests;
    std::cout << "Test 1: Empty graph... ";

    depgraph::Graph graph = {};
    std::vector<int> componentOf = {};
    int C = 0;

    depgraph::Graph dag = depgraph::buildCondensedGraph(graph, componentOf, C);
    std::vector<int> order = depgraph::dependencyFirstOrder(dag);

    assert(dag.empty());
    assert(order.empty());
    assert(verifyOrder(dag, order));

    std::cout << "PASSED\n";
    ++testsPassed;
}

// ────────────────────────────────────────────────────────────
// Case 2: Isolated module
//   "Include its group exactly once."
// ────────────────────────────────────────────────────────────
void testIsolatedModule() {
    ++totalTests;
    std::cout << "Test 2: Isolated module... ";

    depgraph::Graph graph = {{}};
    std::vector<int> componentOf = {0};
    int C = 1;

    depgraph::Graph dag = depgraph::buildCondensedGraph(graph, componentOf, C);
    std::vector<int> order = depgraph::dependencyFirstOrder(dag);

    assert(dag.size() == 1);
    assert(dag[0].empty());
    assert(order.size() == 1);
    assert(order[0] == 0);
    assert(verifyOrder(dag, order));

    std::cout << "PASSED\n";
    ++testsPassed;
}

// ────────────────────────────────────────────────────────────
// Case 3: A -> B -> C (Acyclic chain)
//   "Show dependency-first order C, B, A."
// ────────────────────────────────────────────────────────────
void testAcyclicChain() {
    ++totalTests;
    std::cout << "Test 3: A -> B -> C chain... ";

    // A=0 -> B=1, B=1 -> C=2
    depgraph::Graph graph = {{1}, {2}, {}};
    std::vector<int> componentOf = {0, 1, 2};
    int C = 3;

    depgraph::Graph dag = depgraph::buildCondensedGraph(graph, componentOf, C);
    std::vector<int> order = depgraph::dependencyFirstOrder(dag);

    assert(dag[0] == std::vector<int>{1});
    assert(dag[1] == std::vector<int>{2});
    assert(dag[2].empty());

    // C(2) must come first, then B(1), then A(0)
    assert((order == std::vector<int>{2, 1, 0}));
    assert(verifyOrder(dag, order));

    std::cout << "PASSED\n";
    ++testsPassed;
}

// ────────────────────────────────────────────────────────────
// Case 4: A <-> B, B -> C
//   "Produce groups {A, B} and {C}; show {C} first."
// ────────────────────────────────────────────────────────────
void testCycleWithDependency() {
    ++totalTests;
    std::cout << "Test 4: A <-> B, B -> C... ";

    // A=0 -> B=1, B=1 -> A=0 and C=2, C=2 has no deps
    depgraph::Graph graph = {{1}, {0, 2}, {}};
    // Group 0 = {A, B}, Group 1 = {C}
    std::vector<int> componentOf = {0, 0, 1};
    int C = 2;

    depgraph::Graph dag = depgraph::buildCondensedGraph(graph, componentOf, C);
    std::vector<int> order = depgraph::dependencyFirstOrder(dag);

    // Group 0 depends on Group 1
    assert(dag[0] == std::vector<int>{1});
    assert(dag[1].empty());

    // Group 1 ({C}) must precede Group 0 ({A, B})
    assert((order == std::vector<int>{1, 0}));
    assert(verifyOrder(dag, order));

    std::cout << "PASSED\n";
    ++testsPassed;
}

// ────────────────────────────────────────────────────────────
// Case 5: One module with self-loop
//   "One group, no condensed self-edge; Raj marks the cycle."
// ────────────────────────────────────────────────────────────
void testSelfLoop() {
    ++totalTests;
    std::cout << "Test 5: Self-loop... ";

    depgraph::Graph graph = {{0}};
    std::vector<int> componentOf = {0};
    int C = 1;

    depgraph::Graph dag = depgraph::buildCondensedGraph(graph, componentOf, C);
    std::vector<int> order = depgraph::dependencyFirstOrder(dag);

    // Intra-group edge a == b is dropped, so DAG has no self-edges
    assert(dag.size() == 1);
    assert(dag[0].empty());
    assert((order == std::vector<int>{0}));
    assert(verifyOrder(dag, order));

    std::cout << "PASSED\n";
    ++testsPassed;
}

// ────────────────────────────────────────────────────────────
// Case 6: Disconnected groups
//   "Include every group once; allow multiple valid orders."
// ────────────────────────────────────────────────────────────
void testDisconnectedGroups() {
    ++totalTests;
    std::cout << "Test 6: Disconnected groups... ";

    // Three independent modules
    depgraph::Graph graph = {{}, {}, {}};
    std::vector<int> componentOf = {0, 1, 2};
    int C = 3;

    depgraph::Graph dag = depgraph::buildCondensedGraph(graph, componentOf, C);
    std::vector<int> order = depgraph::dependencyFirstOrder(dag);

    assert(dag.size() == 3);
    assert(dag[0].empty() && dag[1].empty() && dag[2].empty());
    assert(order.size() == 3);
    assert(verifyOrder(dag, order));

    std::cout << "PASSED\n";
    ++testsPassed;
}

// ────────────────────────────────────────────────────────────
// Case 7: Repeated cross-group edges
//   "Indegrees and decrements balance; all groups are processed."
// ────────────────────────────────────────────────────────────
void testRepeatedCrossGroupEdges() {
    ++totalTests;
    std::cout << "Test 7: Repeated cross-group edges... ";

    // Group 0 = {0, 1}, Group 1 = {2, 3}
    // Module 0 -> 2, Module 1 -> 3 (both cross from group 0 to group 1)
    depgraph::Graph graph = {{1, 2}, {0, 3}, {3}, {2}};
    std::vector<int> componentOf = {0, 0, 1, 1};
    int C = 2;

    depgraph::Graph dag = depgraph::buildCondensedGraph(graph, componentOf, C);
    std::vector<int> order = depgraph::dependencyFirstOrder(dag);

    // Duplicate cross-group edges kept in dag[0]
    assert(dag[0].size() == 2);
    assert(dag[0][0] == 1 && dag[0][1] == 1);
    assert(dag[1].empty());

    // Indegrees balance and both groups are returned
    assert((order == std::vector<int>{1, 0}));
    assert(verifyOrder(dag, order));

    std::cout << "PASSED\n";
    ++testsPassed;
}

// ────────────────────────────────────────────────────────────
// Case 8: Full End-to-End integration with Raj's Tarjan output
// ────────────────────────────────────────────────────────────
void testFullEndToEndIntegration() {
    ++totalTests;
    std::cout << "Test 8: Full End-to-End with Raj's Tarjan (examples/handoff)... ";

    const auto modules = depgraph::buildGraph(
        {"App", "Auth", "User", "Database"},
        {{"App", "Auth"},
         {"Auth", "User"},
         {"User", "Auth"},
         {"User", "Database"}});

    const auto scc = depgraph::findSCCs(modules.graph);
    int C = static_cast<int>(scc.groups.size());

    depgraph::Graph dag = depgraph::buildCondensedGraph(modules.graph, scc.componentOf, C);
    std::vector<int> order = depgraph::dependencyFirstOrder(dag);

    assert(verifyOrder(dag, order));

    // Check that Database's group comes before Auth/User's group,
    // and Auth/User's group comes before App's group
    int appComp = scc.componentOf[0];
    int authComp = scc.componentOf[1];
    int dbComp = scc.componentOf[3];

    auto itDb   = std::find(order.begin(), order.end(), dbComp);
    auto itAuth = std::find(order.begin(), order.end(), authComp);
    auto itApp  = std::find(order.begin(), order.end(), appComp);

    assert(itDb < itAuth);
    assert(itAuth < itApp);

    std::cout << "PASSED\n";
    ++testsPassed;
}

}  // namespace

int main() {
    std::cout << "==================================================\n";
    std::cout << "   DepGraph — Dependency Order Verification Suite \n";
    std::cout << "==================================================\n";

    testGuideSample();
    testEmptyGraph();
    testIsolatedModule();
    testAcyclicChain();
    testCycleWithDependency();
    testSelfLoop();
    testDisconnectedGroups();
    testRepeatedCrossGroupEdges();
    testFullEndToEndIntegration();

    std::cout << "==================================================\n";
    std::cout << "Results: " << testsPassed << "/" << totalTests << " passed.\n";
    if (testsPassed == totalTests) {
        std::cout << "ALL TESTS PASSED (Pranav contribution verified)\n";
    } else {
        std::cout << "SOME TESTS FAILED\n";
    }
    std::cout << "==================================================\n";

    return (testsPassed == totalTests) ? 0 : 1;
}

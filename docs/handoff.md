# SCC interface for Pranav

Headers: `graph.h` and `tarjan.h`; link the `depgraph_core` CMake target.

```cpp
const depgraph::ModuleGraph modules = depgraph::buildGraph(
    {"App", "Auth", "User", "Database"},
    {{"App", "Auth"}, {"Auth", "User"},
     {"User", "Auth"}, {"User", "Database"}});
const depgraph::SCCResult scc = depgraph::findSCCs(modules.graph);
// Consume modules.graph, modules.names, scc.groups, scc.componentOf.
```

The actual deterministic result for `examples/handoff.txt` in this version is:

```text
names       = [App, Auth, User, Database]
graph       = [[1], [2], [1, 3], []]
groups      = [[3], [2, 1], [0]]
componentOf = [2, 1, 1, 0]
cyclic      = [false, true, false]
selfLoop    = [false, false, false, false]
```

`groups[c]` holds module IDs; `componentOf[u]` identifies u's group. Component
IDs are contiguous and assigned at extraction. Do not rely on the example's
group order or member order when building condensation or ordering logic.

For every original edge `u -> v`, if `componentOf[u] != componentOf[v]`, append
that component edge to the condensed adjacency list. Preserve each repeated
cross-group edge and count/decrement each occurrence consistently in Kahn's
algorithm. Skip edges within one group.

The example should yield condensed edges `2 -> 1` and `1 -> 0`. Expected
dependency-first output after Pranav's implementation is:
`{Database}, {Auth, User}, {App}`. That ordering is an integration expectation;
the current console program outputs graph, SCC, and cycle results.

`src/main.cpp` currently owns input and display. Integration can add calls after
`findSCCs`, using the same `modules` and `result` values. Coordinate changes to
that file before both members edit it.

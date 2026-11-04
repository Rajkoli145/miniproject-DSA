# Raj's implementation and viva notes

This implements Raj's responsibilities from the contribution guide in C++17.
Pranav owns condensation and Kahn's dependency-first ordering. Those algorithms
remain a separate integration task; SCC extraction order is not their interface.

## Input and module IDs

An edge `A B` means **A depends on B**. Input contains a nonnegative module
count, that many unique module names, a nonnegative edge count, and that many
source/target pairs. Names are case-sensitive whitespace-free tokens. Spaces,
tabs, and newlines separate tokens. Empty input is invalid; an empty graph is
`0 0`. End interactive input with EOF (Ctrl-D on macOS/Linux).

`buildGraph` assigns IDs in the supplied name order and uses a hash table for
name lookup. The adjacency list includes isolated modules. Duplicate names,
unknown endpoints, invalid counts, incomplete input, and trailing tokens are
rejected. Repeated edges and self-loops are retained. `readGraph` handles console
parsing separately; tests and other algorithms can call `findSCCs` directly.

## Tarjan's algorithm

Each DFS visit assigns an increasing discovery number `index[u]` and initializes
`low[u]` to that number. The SCC stack holds discovered vertices that have not
yet been assigned to a completed component; it is distinct from the DFS call
stack. `onStack[u]` tells us whether a vertex is still awaiting assignment.

For an unvisited neighbor, recurse and propagate `low[v]` to `low[u]`. For an
already visited neighbor still on the SCC stack, propagate `index[v]` instead.
Ignore edges to vertices already assigned to completed SCCs.

When `low[u] == index[u]`, `u` is the root of an SCC. Pop through `u`, clear
on-stack flags, and assign each popped vertex the new component ID. Start a DFS
from every unvisited vertex to cover disconnected graphs. State is local to
each `findSCCs` call, so repeated analyses do not reuse previous state.

Example: in `A -> B -> C -> A`, indices are 0, 1, 2. The edge from C to the
still-stacked A lowers `low[C]` to 0. Returning from DFS lowers `low[B]` to 0.
At A, `low[A] == index[A] == 0`, so C, B, and A are popped into one group.

## Cycles and complexity

An SCC with multiple members is cyclic. A singleton is cyclic only when its
module has a self-loop. One adjacency scan records self-loops before traversal.
Output lists mutually dependent groups, not every simple cycle. A group cannot
be given an internal dependency-first build order without resolving its cycle.

Tarjan plus classification takes O(V + E) time and O(V) auxiliary space. Graph
storage is O(V + E). Building the graph uses expected constant-time hash-table
lookups per name or endpoint. This version uses recursive DFS: very deep graphs
can exceed the native call-stack limit. An explicit DFS frame stack would be a
future extension for inputs too deep for recursion.

## Tests and review

`tests/scc_detection.cpp` checks every guide case, mapping invariants, duplicate
edges, input rejection, repeated calls, completed-SCC edges, and sample output.
It also compares every directed graph with 0–4 vertices against an independent
transitive-reachability oracle (66,067 graphs). Tests use runtime checks and
remain active in release builds.

Run the commands in the README before review. A proposed commit message is
`Implement graph input and Tarjan SCC detection`. Publishing a branch, opening
a PR, requesting Pranav's review, and confirming the combined ordering are
collaboration steps still to perform; implementation alone does not complete them.

# Pranav's implementation and viva notes

This document explains Pranav's responsibilities from the contribution guide in C++17:
1. Condensed graph construction (`src/condensation.h`, `src/condensation.cpp`)
2. Kahn's topological sorting and dependency-first ordering (`src/topological_sort.h`, `src/topological_sort.cpp`)
3. Formatting and group display (`src/output.h`, `src/output.cpp`)
4. Verification test suite (`tests/dependency_order.cpp`)

---

## 1. Condensed Graph Construction

### Objective
Once Tarjan's algorithm partitions modules into Strongly Connected Components (SCCs), any internal circular dependency is captured within its respective SCC group. We then collapse the module-level graph into a directed graph of SCC components (one vertex per component).

### Why the Condensed Graph is Acyclic (DAG)
A directed graph of SCCs cannot contain cycles:
If there were a directed cycle between two distinct components $C_1$ and $C_2$ (e.g., $C_1 \to C_2$ and $C_2 \to C_1$), then every module in $C_1$ could reach every module in $C_2$, and vice versa. By the definition of strong connectivity, all modules in $C_1$ and $C_2$ would belong to the **same** maximal SCC. Hence, cross-component cycles cannot exist, guaranteeing that the condensed graph is strictly a Directed Acyclic Graph (DAG).

### Implementation Details
- **Function:** `depgraph::Graph buildCondensedGraph(const Graph& graph, const std::vector<int>& componentOf, int C)`
- Iterates through every module $u \in [0, V-1]$ and its outgoing dependency edges $v \in \text{graph}[u]$.
- Looks up group IDs: $a = \text{componentOf}[u]$ and $b = \text{componentOf}[v]$.
- If $a == b$: the edge is internal to the SCC (intra-group), so it is skipped.
- If $a \neq b$: the cross-group dependency edge $a \to b$ is appended to `dag[a]`.
- **Duplicate cross-group edges:** As required by the contract, parallel cross-group edges are intentionally **retained**. Each parallel edge increments the destination component's indegree and will be decremented individually in Kahn's algorithm, keeping indegree counts and decrements balanced in $O(V + E)$ time without requiring a hash set deduplication overhead.

### Complexity
- **Time Complexity:** $O(V + E)$ — scans every module and every edge in the adjacency list exactly once.
- **Space Complexity:** $O(C + E_c)$ — stores $C$ component adjacency lists with $E_c$ cross-group edges ($E_c \le E$).

---

## 2. Kahn's Topological Sorting (Dependency-First Order)

### Objective
Given the condensed DAG, find an ordering of SCC groups such that for every dependency relation, the prerequisite dependency appears **before** the dependent module (i.e., build-system or installation order).

### Edge Convention and Reversal
- In this project, directed edge $A \to B$ means **"$A$ depends on $B$"**.
- Therefore, $A$ is the *dependent* and $B$ is the *dependency*.
- In standard Kahn's algorithm:
  - Nodes with indegree 0 (nodes with no incoming edges) are processed first.
  - Since $A \to B$ directs an edge into $B$, $B$'s indegree is higher than $A$'s.
  - Thus, standard Kahn's algorithm outputs $A$ (dependent) before $B$ (dependency).
- To achieve **dependency-first ordering** (compiling libraries before applications), we **reverse** the completed topological sort array:
  $$\text{Dependency-First Order} = \text{reverse}(\text{Kahn's Order})$$

### Algorithm Steps
1. **Compute Indegrees:**
   Initialize `indegree[0..C-1] = 0`. For each group $a$ and edge $a \to b$ in `dag[a]`, increment `indegree[b]++`.
2. **Initialize Queue:**
   Enqueue every component $i \in [0, C-1]$ where `indegree[i] == 0`.
3. **BFS Traversal:**
   While the queue is nonempty:
   - Dequeue component `curr` and append to `order`.
   - For every neighbor $b \in \text{dag}[curr]$, decrement `indegree[b]--`.
   - If `indegree[b] == 0`, enqueue $b$.
4. **Acyclicity Verification:**
   If `order.size() != C`, fewer than $C$ components were processed. This signals a cycle in the condensed graph (an internal inconsistency), returning an empty order.
5. **Reversal:**
   Reverse `order` in-place using `std::reverse(order.begin(), order.end())`.

### Edge Cases Handled
- **Empty graph ($V = 0, C = 0$):** Returns an empty order without exceptions or crashes.
- **Isolated modules:** Have indegree 0; enqueued at start and included exactly once.
- **Disconnected components:** Handled seamlessly because all components with initial indegree 0 enter the queue.
- **Self-loops:** Suppressed during condensation ($a == b$), so they do not produce self-edges in the DAG.
- **Repeated edges:** Handled cleanly as each duplicate edge increments indegree by 1 and is decremented once when the predecessor is dequeued.

### Complexity
- **Time Complexity:** $O(C + E_c)$ — each component is enqueued/dequeued once, and each condensed edge is traversed once.
- **Space Complexity:** $O(C)$ — indegree array, BFS queue, and result order vector.

---

## 3. Total Complexity for Pranav's Contribution

$$\text{Time: } O(V + E) \quad | \quad \text{Auxiliary Space: } O(V + E)$$

Because $C \le V$ and $E_c \le E$, the linear condensation scan dominates, ensuring overall optimal linear time and memory usage.

---

## 4. Worked Example Trace

Consider the canonical project sample:
```text
Modules: App (0), Auth (1), User (2), Database (3)
Dependencies:
  App -> Auth
  Auth -> User
  User -> Auth      (Auth <-> User form a cycle)
  User -> Database
```

### Tarjan SCC Output:
- Group 0 = `{Database}`
- Group 1 = `{User, Auth}` (cyclic)
- Group 2 = `{App}`
- `componentOf = [2, 1, 1, 0]`

### Condensation Trace:
- Edge `App(2) -> Auth(1)`: cross-group edge $\to$ `dag[2]` gets `1`.
- Edge `Auth(1) -> User(1)`: intra-group ($1 == 1$) $\to$ skipped.
- Edge `User(1) -> Auth(1)`: intra-group ($1 == 1$) $\to$ skipped.
- Edge `User(1) -> Database(0)`: cross-group edge $\to$ `dag[1]` gets `0`.
- Resulting DAG:
  - `dag[0] = []`
  - `dag[1] = [0]`
  - `dag[2] = [1]`

### Kahn's Algorithm Trace:
- Initial Indegrees: `indegree[0] = 1`, `indegree[1] = 1`, `indegree[2] = 0`.
- Queue initially: `[2]`.
- Dequeue `2`:
  - `order = [2]`
  - Neighbor `1`: `indegree[1]` becomes $0 \to$ enqueue `1`.
- Dequeue `1`:
  - `order = [2, 1]`
  - Neighbor `0`: `indegree[0]` becomes $0 \to$ enqueue `0`.
- Dequeue `0`:
  - `order = [2, 1, 0]`
- Reversal:
  - `order` reversed $\to [0, 1, 2]$
- Readable mapping:
  - Group 0: `{Database}`
  - Group 1: `{User, Auth}`
  - Group 2: `{App}`
  - Final Output: `{Database}, {User, Auth}, {App}`

---

## 5. Test Suite (`tests/dependency_order.cpp`)

The test executable validates 9 distinct scenarios:
1. `Test 0`: Guide Sample (App, Auth, User, Database)
2. `Test 1`: Empty graph ($V=0, C=0$)
3. `Test 2`: Isolated single module
4. `Test 3`: Acyclic chain $A \to B \to C$
5. `Test 4`: Cycle with external dependency ($A \leftrightarrow B, B \to C$)
6. `Test 5`: Self-loop module ($A \to A$)
7. `Test 6`: Disconnected groups (multiple valid topological orders)
8. `Test 7`: Repeated cross-group edges (indegree balancing)
9. `Test 8`: End-to-end integration test combining `depgraph::findSCCs` and `buildGraph` with `buildCondensedGraph` and `dependencyFirstOrder`.

Every test verifies that:
- Every component $0 \dots C-1$ appears exactly once.
- For every condensed edge $a \to b$, $b$ appears strictly **before** $a$ in the final ordering.

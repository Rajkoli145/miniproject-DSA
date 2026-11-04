# DepGraph — Software Dependency Cycle Analyzer

A two-person Data Structures and Algorithms mini project for detecting circular software dependencies using directed graphs and Tarjan’s strongly connected components algorithm.

## Project status

Planning stage. This README defines the proposed implementation; the features below are not yet implemented. The programming language and build/run commands will be added once the team confirms the course requirements.

## Problem statement

Software modules often depend on other modules. Circular dependencies occur when a chain of dependencies leads back to its starting module. These cycles can make software harder to build, maintain, and test.

DepGraph will accept dependency data, identify groups of mutually dependent modules, and construct a simplified acyclic graph of those groups. It will then report a dependency-first ordering of the groups.

## Objectives

- Represent dependencies using a directed graph and adjacency lists.
- Implement Tarjan’s algorithm to find strongly connected components (SCCs).
- Identify cyclic SCCs and self-loops.
- Construct a condensed directed acyclic graph (DAG).
- Implement topological sorting to produce a dependency-first group order.
- Demonstrate the algorithms on normal and edge-case inputs.

## Planned features

- Enter module names and directed dependencies.
- Display the adjacency list and detected SCCs.
- Highlight cyclic groups and modules that depend on themselves.
- Collapse each SCC into a single group in the condensed graph.
- Display a valid dependency-first order of those groups.
- Optional extension: visualize the original and condensed graphs.

The initial version will use supplied dependency data. Automatic source-code import analysis is outside the initial scope.

## Data structures and algorithms

| Component | Data structure / algorithm | Purpose |
| --- | --- | --- |
| Dependency representation | Adjacency list | Store outgoing dependency edges |
| SCC detection | DFS and Tarjan’s algorithm | Find mutually reachable module groups |
| Tarjan state | Discovery and low-link arrays, stack, on-stack flags | Track traversal and SCC boundaries |
| Graph condensation | Component ID array and adjacency lists | Replace each SCC with one group |
| Group ordering | Kahn’s algorithm and a queue | Topologically order the condensed graph |

Core graph algorithms will be implemented by the team rather than delegated to graph-algorithm libraries.

## Edge convention and ordering

An edge `A -> B` means **A depends on B**. The same convention applies to the condensed graph.

With this convention, ordinary topological sorting puts dependents before dependencies. Reverse the resulting topological order to display a **dependency-first** order. Alternatively, topologically sort a reversed condensed graph.

SCCs with more than one module are cyclic. A single-module SCC is cyclic only if that module has a self-loop.

Collapsing a cycle does not fix it: the output gives an order between groups, not a valid build order inside a cyclic group.

## Example

Input dependencies:

```text
App -> Auth
Auth -> User
User -> Auth
User -> Database
```

Expected analysis:

```text
Strongly connected components:
{App}
{Auth, User}
{Database}

Cyclic group:
{Auth, User}

Condensed dependencies:
{App} -> {Auth, User}
{Auth, User} -> {Database}

Dependency-first group order:
{Database}, {Auth, User}, {App}
```

The order in which Tarjan reports SCCs, and the order of modules within an SCC, may vary. Independent groups can also have multiple valid orderings.

## Shared implementation contract

- Assign each module an integer ID from `0` to `V - 1` and preserve a mapping to its name.
- Store `graph[u]` as the list of modules that module `u` depends on.
- Return SCCs as a list of lists of module IDs.
- Assign every vertex a component ID during SCC extraction.
- Pass the original graph, SCC groups, and component IDs to the condensation code.
- Ignore edges within the same SCC when constructing the condensed graph.
- Keep one condensed edge per original cross-component edge, including parallel edges, and count/decrement each occurrence consistently in Kahn’s algorithm. This keeps construction linear without requiring edge deduplication.
- Accept disconnected and isolated modules; reject unknown module references.

## Work distribution

| Responsibility | Raj — Member 1 | Member 2 — add name |
| --- | --- | --- |
| Graph representation | Adjacency list, module IDs, input validation | Review shared input/output contract |
| Main algorithm | Tarjan’s SCC detection and cycle classification | Condensation and Kahn’s topological sorting |
| Output | SCCs and cyclic groups | Condensed dependencies and dependency-first ordering |
| Verification | SCC cases, self-loops, invalid references | Condensation and ordering cases |
| Documentation | Explain DFS, stack, discovery and low-link values | Explain DAGs, indegrees, queues and ordering |

Both members will integrate the program, review each other’s changes, and prepare the final report and demonstration. One member should own the initial entry-point file to avoid overlapping edits.

## Suggested repository organization

Use source-file extensions appropriate to the chosen language.

| Location | Contents |
| --- | --- |
| `src/` | Main program, graph representation, Tarjan, condensation, ordering |
| `examples/` | Small input files and their expected analyses |
| `tests/` | Algorithm and integration checks |
| `docs/` | Algorithm explanations and project report |
| `README.md` | Project overview and collaboration instructions |

## Git collaboration

1. Create one shared repository and add both members as collaborators.
2. Agree on the language, file names, and shared contract before coding.
3. Raj works on `feature/scc-detection`.
4. Member 2 works on `feature/dependency-order`, initially using sample SCC results.
5. Commit small working changes and push the feature branches.
6. Open pull requests into `main`; the other member reviews the code and checks the examples before merging.
7. Pull the latest `main` and merge it into an unfinished feature branch when shared changes are needed.

Example commands for Raj after cloning the repository:

```bash
git switch -c feature/scc-detection
# Implement and check a working change.
git add src/
git commit -m "Implement Tarjan SCC detection"
git push -u origin feature/scc-detection
```

Member 2 uses `feature/dependency-order` instead. Commit or otherwise save local work before switching branches. Open and review the pull request on GitHub after pushing.

## Verification plan

| Input case | Expected behavior |
| --- | --- |
| No modules | Empty result with a clear message |
| One isolated module | One non-cyclic SCC |
| Acyclic chain | No cycles; dependencies appear first |
| Three-module cycle | One cyclic SCC containing all three modules |
| Self-loop | Single-module SCC marked cyclic |
| Disconnected graph | Every module appears in exactly one SCC |
| Multiple cycles connected by edges | Separate SCC groups with valid inter-group ordering |
| Repeated dependency edge | No incorrect indegree counts or missing output groups |
| Unknown module reference | Clear validation error |

Integration checks: every vertex belongs to exactly one SCC, the condensed graph is acyclic, and every dependency group precedes its dependent group in the displayed order.

## Complexity

Let `V` be the number of modules and `E` the number of supplied dependency edges. Let `C` and `Ec` be the vertices and edges of the condensed graph.

| Operation | Time |
| --- | --- |
| Build adjacency lists | O(V + E) |
| Tarjan’s algorithm | O(V + E) |
| Construct condensed graph using component IDs | O(V + E) |
| Kahn’s topological sorting | O(C + Ec) |

The core analysis takes O(V + E) time and O(V + E) total storage, including the graph. Name lookup, input parsing, optional sorting, and visualization costs depend on the implementation. Recursive Tarjan implementations may also be limited by the language’s call-stack depth.

## Completion checklist

- [ ] Confirm language and course requirements.
- [ ] Set up the repository and shared function interfaces.
- [ ] Implement and check Tarjan’s algorithm.
- [ ] Implement condensation and dependency-first ordering.
- [ ] Integrate input handling and output.
- [ ] Run the verification cases.
- [ ] Add exact build/run commands for the implemented program.
- [ ] Prepare the report and viva demonstration.

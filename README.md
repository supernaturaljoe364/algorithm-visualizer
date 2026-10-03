# Algorithm Visualizer

Dijkstra's shortest-path algorithm compared with BFS on a small weighted graph. The C++ backend
runs both, records every step, and serves the results as JSON for the web frontend.

| Folder      | What                                                         |
|-------------|--------------------------------------------------------------|
| `backend/`  | C++20: graph, Dijkstra, BFS, HTTP server, tests, benchmarks  |
| `frontend/` | HTML/CSS/JS (frontend team)                                  |
| `docs/`     | `API.md` (the JSON contract), `FRONTEND_PROMPT.md`           |

## Build and run

Needs a C++20 compiler and CMake 3.20+. The first configure downloads Crow (HTTP) and Asio into
`build/_deps` (internet needed once; nothing is installed system-wide).

```bash
cmake -S . -B build && cmake --build build
./build/server            # http://localhost:18080 : the API, plus the files in frontend/
./build/tests
./build/benchmarks
```

```bash
curl localhost:18080/api/graph
curl -X POST localhost:18080/api/dijkstra -d '{"source":0,"target":7}'
curl -X POST localhost:18080/api/bfs      -d '{"source":0,"target":7}'
```

## Code (`backend/`)

| File | What it is |
|------|------------|
| `include/graph.h` | Adjacency-list graph (undirected, non-negative weights) |
| `include/algorithms.h` | `Step`, `Stats`, `PathResult`, and the `dijkstra` / `bfs` declarations |
| `src/dijkstra.cpp` | Dijkstra with `std::priority_queue`; the 10 stages are numbered in comments |
| `src/bfs.cpp` | BFS with a plain `std::queue` |
| `include/demo_graph.h` | The fixed 8-node graph served to the frontend |
| `src/main.cpp` | HTTP layer (Crow): parse JSON request, call the algorithm, write JSON |
| `tests/`, `benchmarks/` | 16 tests; a benchmark table with operation counts |

The algorithms never touch HTTP or JSON; only `main.cpp` does.

## How the algorithms work

**Dijkstra** keeps the best known distance to every node and a min-heap of `(distance, node)`.
It pops the closest entry. That node's distance is now final, since weights are never negative.
Then it *relaxes* each edge `u -> v`: if `dist[u] + w < dist[v]`, it updates `dist[v]` and the
predecessor, and pushes the new entry. `priority_queue` cannot decrease a key, so the old entry
for `v` stays in the heap; when popped later it is **stale** (its distance is larger than
`dist[v]`) and is skipped. Time O((V + E) log V).

**BFS** uses a plain queue and reaches nodes in order of edge count, ignoring weights. It finds
the path with the fewest edges, not the cheapest. Time O(V + E).

On the demo graph, A to H:

| | Path | Edges | Cost |
|---|------|-------|------|
| BFS | A → D → H | 2 | 16 |
| Dijkstra | A → C → E → G → H | 4 | 10 |

## Steps

Each run records `visit`, `examine`, `relax`, `stale` (Dijkstra only) and `done` events, so the
frontend can replay the algorithm one step at a time. Format: [`docs/API.md`](docs/API.md).

## Benchmarks

`./build/benchmarks` runs both algorithms on random connected graphs with an unreachable target,
so the whole graph is explored (worst case); step recording is off and times are medians.
Results from this machine: BFS is about 2–4x faster. Dijkstra does the same work per edge plus
heap operations, and about 44% of its heap pops are stale entries.

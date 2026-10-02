# Backend API Contract

This is the contract between the C++ backend (`backend/`, owned by Dane) and the
JavaScript frontend (`frontend/`, owned by the frontend team).

**This file is the source of truth.** If the backend output ever disagrees with this
file, tell Dane. Only Dane edits this file.

> Status: contract defined; the HTTP server is still being built. Until it is ready,
> the frontend should use the sample responses below as mock data.

---

## Server

- Address: `http://localhost:18080`
- The backend also serves the `frontend/` folder, so opening `http://localhost:18080/`
  loads `frontend/index.html`. API calls can then use relative paths like `/api/graph`.
- CORS is enabled, so a page opened another way can still call the API.
- All bodies are JSON (`Content-Type: application/json`).

## Concepts

- Nodes are identified by an integer `id` (0, 1, 2, ...). Each node also has a
  display `label` ("A", "B", ...). **Requests and responses always use ids**; use the
  graph's labels for display.
- Edges are **undirected** and have a non-negative integer `weight`.
- The graph is fixed and defined by the backend. The frontend only chooses a
  source, a target and an algorithm.

---

## `GET /api/graph`

Returns the graph to draw. `x`/`y` are pixel positions inside a `width` x `height`
canvas (use them directly as SVG coordinates with `viewBox="0 0 width height"`).

```json
{
  "width": 840,
  "height": 500,
  "nodes": [
    { "id": 0, "label": "A", "x": 80,  "y": 250 },
    { "id": 1, "label": "B", "x": 240, "y": 100 },
    { "id": 2, "label": "C", "x": 240, "y": 400 },
    { "id": 3, "label": "D", "x": 420, "y": 180 },
    { "id": 4, "label": "E", "x": 420, "y": 340 },
    { "id": 5, "label": "F", "x": 620, "y": 450 },
    { "id": 6, "label": "G", "x": 590, "y": 300 },
    { "id": 7, "label": "H", "x": 760, "y": 250 }
  ],
  "edges": [
    { "from": 0, "to": 1, "weight": 4 },
    { "from": 0, "to": 2, "weight": 2 },
    { "from": 0, "to": 3, "weight": 7 },
    { "from": 1, "to": 3, "weight": 2 },
    { "from": 1, "to": 4, "weight": 6 },
    { "from": 2, "to": 3, "weight": 5 },
    { "from": 2, "to": 4, "weight": 3 },
    { "from": 3, "to": 6, "weight": 4 },
    { "from": 3, "to": 7, "weight": 9 },
    { "from": 4, "to": 5, "weight": 4 },
    { "from": 4, "to": 6, "weight": 2 },
    { "from": 5, "to": 7, "weight": 2 },
    { "from": 6, "to": 7, "weight": 3 }
  ]
}
```

## `POST /api/dijkstra` and `POST /api/bfs`

Both endpoints take the same request and return the same response shape.

Request:

```json
{ "source": 0, "target": 7 }
```

Response fields:

| Field       | Type            | Meaning |
|-------------|-----------------|---------|
| `algorithm` | string          | `"dijkstra"` or `"bfs"` |
| `source`    | int             | Echo of the request |
| `target`    | int             | Echo of the request |
| `found`     | bool            | Whether a path exists |
| `path`      | int[]           | Node ids from source to target; `[]` if not found |
| `distance`  | int or null     | Total **weight** of `path`; `null` if not found |
| `hops`      | int or null     | Number of **edges** in `path`; `null` if not found |
| `steps`     | Step[]          | What the algorithm did, in order (see below) |
| `stats`     | object          | Counters for the statistics panel (see below) |

Note: BFS ignores weights while searching (it minimises `hops`), but `distance` is
still the total weight of the path it found, so the two algorithms can be compared.

### Steps

Each step has a `type`, a ready-to-display `message`, and type-specific fields.
Play them in array order. **Ignore any `type` you don't recognise.**

| `type`    | Fields                       | Meaning | Suggested visual |
|-----------|------------------------------|---------|------------------|
| `visit`   | `node`, `distance`           | Algorithm takes `node` out of its queue and processes it. For Dijkstra its distance is now final. | Previous current node becomes "visited"; this node becomes "current"; show `distance` on it. |
| `examine` | `from`, `to`, `weight`       | Looks at the edge `from` -> `to`. | Briefly highlight the edge. |
| `relax`   | `from`, `to`, `distance`     | Found a better route to `to` (through `from`); `to`'s distance becomes `distance`. | Show new distance on `to`; mark `to` as "frontier"; mark the edge as "relaxed". |
| `stale`   | `node`, `distance`           | Dijkstra only: popped an outdated queue entry for `node` and skipped it. | Show the message; optional brief flash of the node. |
| `done`    | `found`                      | Algorithm finished. | Highlight `path` (nodes and edges). |

For BFS, `distance` in `visit`/`relax` is the number of edges from the source.

### Stats

| Field                  | Meaning |
|------------------------|---------|
| `nodesVisited`         | Number of `visit` steps |
| `edgesExamined`        | Number of `examine` steps |
| `edgesRelaxed`         | Number of `relax` steps |
| `queuePushes`          | Pushes into the priority queue (Dijkstra) or FIFO queue (BFS) |
| `queuePops`            | Pops from that queue |
| `staleEntriesSkipped`  | Number of `stale` steps (always 0 for BFS) |
| `timeMicroseconds`     | Algorithm run time (varies between runs; number, may be fractional) |

### Errors

Invalid requests return HTTP `400` with:

```json
{ "error": "source node 42 does not exist" }
```

### Special cases

- `source == target`: `found: true`, `path: [source]`, `distance: 0`, `hops: 0`.
- Unreachable target: `found: false`, `path: []`, `distance: null`, `hops: null`, and the
  `done` step has `found: false`. (The demo graph is connected, so this won't happen there.)

---

## Sample: `POST /api/dijkstra` with `{ "source": 0, "target": 7 }` (A to H)

```json
{
  "algorithm": "dijkstra",
  "source": 0,
  "target": 7,
  "found": true,
  "path": [0, 2, 4, 6, 7],
  "distance": 10,
  "hops": 4,
  "steps": [
    { "type": "visit",   "node": 0, "distance": 0, "message": "Visit A (distance 0)" },
    { "type": "examine", "from": 0, "to": 1, "weight": 4, "message": "Examine edge A -> B (weight 4)" },
    { "type": "relax",   "from": 0, "to": 1, "distance": 4, "message": "Update B: distance 4 via A" },
    { "type": "examine", "from": 0, "to": 2, "weight": 2, "message": "Examine edge A -> C (weight 2)" },
    { "type": "relax",   "from": 0, "to": 2, "distance": 2, "message": "Update C: distance 2 via A" },
    { "type": "examine", "from": 0, "to": 3, "weight": 7, "message": "Examine edge A -> D (weight 7)" },
    { "type": "relax",   "from": 0, "to": 3, "distance": 7, "message": "Update D: distance 7 via A" },
    { "type": "visit",   "node": 2, "distance": 2, "message": "Visit C (distance 2)" },
    { "type": "examine", "from": 2, "to": 0, "weight": 2, "message": "Examine edge C -> A (weight 2)" },
    { "type": "examine", "from": 2, "to": 3, "weight": 5, "message": "Examine edge C -> D (weight 5)" },
    { "type": "examine", "from": 2, "to": 4, "weight": 3, "message": "Examine edge C -> E (weight 3)" },
    { "type": "relax",   "from": 2, "to": 4, "distance": 5, "message": "Update E: distance 5 via C" },
    { "type": "visit",   "node": 1, "distance": 4, "message": "Visit B (distance 4)" },
    { "type": "examine", "from": 1, "to": 0, "weight": 4, "message": "Examine edge B -> A (weight 4)" },
    { "type": "examine", "from": 1, "to": 3, "weight": 2, "message": "Examine edge B -> D (weight 2)" },
    { "type": "relax",   "from": 1, "to": 3, "distance": 6, "message": "Update D: distance 6 via B" },
    { "type": "examine", "from": 1, "to": 4, "weight": 6, "message": "Examine edge B -> E (weight 6)" },
    { "type": "visit",   "node": 4, "distance": 5, "message": "Visit E (distance 5)" },
    { "type": "examine", "from": 4, "to": 1, "weight": 6, "message": "Examine edge E -> B (weight 6)" },
    { "type": "examine", "from": 4, "to": 2, "weight": 3, "message": "Examine edge E -> C (weight 3)" },
    { "type": "examine", "from": 4, "to": 5, "weight": 4, "message": "Examine edge E -> F (weight 4)" },
    { "type": "relax",   "from": 4, "to": 5, "distance": 9, "message": "Update F: distance 9 via E" },
    { "type": "examine", "from": 4, "to": 6, "weight": 2, "message": "Examine edge E -> G (weight 2)" },
    { "type": "relax",   "from": 4, "to": 6, "distance": 7, "message": "Update G: distance 7 via E" },
    { "type": "visit",   "node": 3, "distance": 6, "message": "Visit D (distance 6)" },
    { "type": "examine", "from": 3, "to": 0, "weight": 7, "message": "Examine edge D -> A (weight 7)" },
    { "type": "examine", "from": 3, "to": 1, "weight": 2, "message": "Examine edge D -> B (weight 2)" },
    { "type": "examine", "from": 3, "to": 2, "weight": 5, "message": "Examine edge D -> C (weight 5)" },
    { "type": "examine", "from": 3, "to": 6, "weight": 4, "message": "Examine edge D -> G (weight 4)" },
    { "type": "examine", "from": 3, "to": 7, "weight": 9, "message": "Examine edge D -> H (weight 9)" },
    { "type": "relax",   "from": 3, "to": 7, "distance": 15, "message": "Update H: distance 15 via D" },
    { "type": "stale",   "node": 3, "distance": 7, "message": "Skip stale queue entry for D (7 > 6)" },
    { "type": "visit",   "node": 6, "distance": 7, "message": "Visit G (distance 7)" },
    { "type": "examine", "from": 6, "to": 3, "weight": 4, "message": "Examine edge G -> D (weight 4)" },
    { "type": "examine", "from": 6, "to": 4, "weight": 2, "message": "Examine edge G -> E (weight 2)" },
    { "type": "examine", "from": 6, "to": 7, "weight": 3, "message": "Examine edge G -> H (weight 3)" },
    { "type": "relax",   "from": 6, "to": 7, "distance": 10, "message": "Update H: distance 10 via G" },
    { "type": "visit",   "node": 5, "distance": 9, "message": "Visit F (distance 9)" },
    { "type": "examine", "from": 5, "to": 4, "weight": 4, "message": "Examine edge F -> E (weight 4)" },
    { "type": "examine", "from": 5, "to": 7, "weight": 2, "message": "Examine edge F -> H (weight 2)" },
    { "type": "visit",   "node": 7, "distance": 10, "message": "Visit H (distance 10)" },
    { "type": "done",    "found": true, "message": "Reached H: shortest distance 10" }
  ],
  "stats": {
    "nodesVisited": 8,
    "edgesExamined": 23,
    "edgesRelaxed": 9,
    "queuePushes": 10,
    "queuePops": 9,
    "staleEntriesSkipped": 1,
    "timeMicroseconds": 3.2
  }
}
```

## Sample: `POST /api/bfs` with `{ "source": 0, "target": 7 }` (A to H)

```json
{
  "algorithm": "bfs",
  "source": 0,
  "target": 7,
  "found": true,
  "path": [0, 3, 7],
  "distance": 16,
  "hops": 2,
  "steps": [
    { "type": "visit",   "node": 0, "distance": 0, "message": "Visit A (0 edges from source)" },
    { "type": "examine", "from": 0, "to": 1, "weight": 4, "message": "Examine edge A -> B" },
    { "type": "relax",   "from": 0, "to": 1, "distance": 1, "message": "Discover B: 1 edge via A" },
    { "type": "examine", "from": 0, "to": 2, "weight": 2, "message": "Examine edge A -> C" },
    { "type": "relax",   "from": 0, "to": 2, "distance": 1, "message": "Discover C: 1 edge via A" },
    { "type": "examine", "from": 0, "to": 3, "weight": 7, "message": "Examine edge A -> D" },
    { "type": "relax",   "from": 0, "to": 3, "distance": 1, "message": "Discover D: 1 edge via A" },
    { "type": "visit",   "node": 1, "distance": 1, "message": "Visit B (1 edge from source)" },
    { "type": "examine", "from": 1, "to": 0, "weight": 4, "message": "Examine edge B -> A" },
    { "type": "examine", "from": 1, "to": 3, "weight": 2, "message": "Examine edge B -> D" },
    { "type": "examine", "from": 1, "to": 4, "weight": 6, "message": "Examine edge B -> E" },
    { "type": "relax",   "from": 1, "to": 4, "distance": 2, "message": "Discover E: 2 edges via B" },
    { "type": "visit",   "node": 2, "distance": 1, "message": "Visit C (1 edge from source)" },
    { "type": "examine", "from": 2, "to": 0, "weight": 2, "message": "Examine edge C -> A" },
    { "type": "examine", "from": 2, "to": 3, "weight": 5, "message": "Examine edge C -> D" },
    { "type": "examine", "from": 2, "to": 4, "weight": 3, "message": "Examine edge C -> E" },
    { "type": "visit",   "node": 3, "distance": 1, "message": "Visit D (1 edge from source)" },
    { "type": "examine", "from": 3, "to": 0, "weight": 7, "message": "Examine edge D -> A" },
    { "type": "examine", "from": 3, "to": 1, "weight": 2, "message": "Examine edge D -> B" },
    { "type": "examine", "from": 3, "to": 2, "weight": 5, "message": "Examine edge D -> C" },
    { "type": "examine", "from": 3, "to": 6, "weight": 4, "message": "Examine edge D -> G" },
    { "type": "relax",   "from": 3, "to": 6, "distance": 2, "message": "Discover G: 2 edges via D" },
    { "type": "examine", "from": 3, "to": 7, "weight": 9, "message": "Examine edge D -> H" },
    { "type": "relax",   "from": 3, "to": 7, "distance": 2, "message": "Discover H: 2 edges via D" },
    { "type": "visit",   "node": 4, "distance": 2, "message": "Visit E (2 edges from source)" },
    { "type": "examine", "from": 4, "to": 1, "weight": 6, "message": "Examine edge E -> B" },
    { "type": "examine", "from": 4, "to": 2, "weight": 3, "message": "Examine edge E -> C" },
    { "type": "examine", "from": 4, "to": 5, "weight": 4, "message": "Examine edge E -> F" },
    { "type": "relax",   "from": 4, "to": 5, "distance": 3, "message": "Discover F: 3 edges via E" },
    { "type": "examine", "from": 4, "to": 6, "weight": 2, "message": "Examine edge E -> G" },
    { "type": "visit",   "node": 6, "distance": 2, "message": "Visit G (2 edges from source)" },
    { "type": "examine", "from": 6, "to": 3, "weight": 4, "message": "Examine edge G -> D" },
    { "type": "examine", "from": 6, "to": 4, "weight": 2, "message": "Examine edge G -> E" },
    { "type": "examine", "from": 6, "to": 7, "weight": 3, "message": "Examine edge G -> H" },
    { "type": "visit",   "node": 7, "distance": 2, "message": "Visit H (2 edges from source)" },
    { "type": "done",    "found": true, "message": "Reached H in 2 edges (path cost 16)" }
  ],
  "stats": {
    "nodesVisited": 7,
    "edgesExamined": 21,
    "edgesRelaxed": 7,
    "queuePushes": 8,
    "queuePops": 7,
    "staleEntriesSkipped": 0,
    "timeMicroseconds": 1.4
  }
}
```

The two samples show the point of the project: BFS finds A -> D -> H (fewest edges,
cost 16) while Dijkstra finds A -> C -> E -> G -> H (more edges, cost 10).

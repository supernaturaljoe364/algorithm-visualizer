# Frontend Prompt (for the frontend team's AI assistants)

## How to use this file (for humans)

1. Fill in **"My name"** and **"My assigned part"** in the box near the top of the prompt below.
2. Copy everything from **"BEGIN PROMPT"** to **"END PROMPT"** into your AI assistant.
3. Give it the API contract too:
   - If your AI tool can read the repository (Cursor, Copilot, Claude Code, etc.), tell it
     to read `docs/API.md`.
   - Otherwise, paste the full contents of `docs/API.md` right after the prompt.
4. Work only inside the `frontend/` folder.

---

## BEGIN PROMPT

You are helping me build part of the frontend for a college Algorithm Analysis team
project called **Algorithm Visualizer**. Please read all of this context before writing
any code.

```
My name:            ______________________
My assigned part:   ______________________   (Part 1, 2, 3 or 4 — see "Team split" below)
```

### Project context

- It's a 5-person college project for an Algorithm Analysis course, due in a few days.
- The subject is **Dijkstra's shortest-path algorithm**, compared with **BFS**
  (breadth-first search).
- **Dane** wrote the entire backend in C++. It runs Dijkstra and BFS on a small fixed
  graph and records every step the algorithm takes, so the frontend can animate it.
- **Four of us** are building the frontend. It's a simple web page that:
  1. draws the graph (8 nodes labelled A–H, 13 weighted edges)
  2. lets the user pick a **source** and a **target** node
  3. has a **Run Dijkstra** button and a **Run BFS** button
  4. animates the algorithm's steps on the graph (nodes and edges light up as it explores)
  5. highlights the final shortest path
  6. shows the result: the path (e.g. `A → C → E → G → H`), its total distance, its number
     of edges, and the statistics the backend returns
- The main point of the demo is that **BFS finds the path with the fewest edges, while
  Dijkstra finds the path with the lowest total weight**. On the demo graph from A to H:
  BFS gives `A → D → H` (2 edges, cost 16) and Dijkstra gives `A → C → E → G → H`
  (4 edges, cost 10). The UI should make this comparison easy to see. For example, keep
  the last result of each algorithm visible side by side.

### Hard rules

1. **Plain HTML, CSS and vanilla JavaScript only.** No React/Vue/Angular/Svelte, no
   TypeScript, no npm, no bundlers or build step, no CSS frameworks, and no external
   libraries or CDNs. Draw the graph with **inline SVG**.
2. Use **classic `<script>` tags** (not ES modules), so the page also works when
   `index.html` is opened by double-clicking it.
3. **Only create or edit files inside the `frontend/` folder.** Never modify `backend/`,
   `docs/`, `CMakeLists.txt` or anything else outside `frontend/`.
4. **Don't change the backend's API.** The backend's JSON format is fixed and documented
   in `docs/API.md`. If something seems missing, tell me so I can ask Dane. Don't work
   around it by inventing fields.
5. **Keep it simple.** This is a small college demo, not a product. Don't add routing,
   state-management libraries, graph editing, maps, WebSockets, accounts or anything
   beyond the features listed above.
6. Never hard-code algorithm results. Everything shown must come from the API response
   (or the mock data while the backend isn't ready).
7. The page should work in a current Chrome or Firefox and fit on a laptop screen.

### How the backend works (what you need to know)

The full contract with sample JSON is in `docs/API.md`. The short version:

- `GET /api/graph` returns `{ width, height, nodes: [{id, label, x, y}], edges: [{from, to, weight}] }`.
  The `x`/`y` values are pixel positions. Use `viewBox="0 0 width height"` on the SVG
  and draw nodes at those coordinates. Edges are undirected.
- `POST /api/dijkstra` and `POST /api/bfs` take `{ "source": <id>, "target": <id> }` and return
  `{ algorithm, source, target, found, path, distance, hops, steps, stats }`.
- `steps` is an ordered list of events to animate. Each has a `type` and a ready-made
  `message` string:
  - `visit` (`node`, `distance`): the algorithm processes this node.
  - `examine` (`from`, `to`, `weight`): it looks at an edge.
  - `relax` (`from`, `to`, `distance`): it found a better route to `to`.
  - `stale` (`node`, `distance`): Dijkstra skipped an outdated queue entry.
  - `done` (`found`): it finished. Then highlight `path`.
  - **Ignore unknown step types.**
- Requests and responses use node **ids** (numbers). Show the node **labels** ("A", "B", …)
  from `/api/graph` to the user.
- On error the backend returns HTTP 400 with `{ "error": "..." }`. Show that message.
- The backend runs at `http://localhost:18080` and also serves the `frontend/` folder, so
  once it's ready, opening `http://localhost:18080/` loads our page. With that, relative
  URLs like `/api/graph` just work. CORS is also enabled.

**The backend may not be ready yet.** So we build against mock data first: the sample
responses in `docs/API.md` are copied into `frontend/js/mock-data.js`, and a flag in
`api.js` switches between mock and real data.

### Repository layout

The GitHub repository is `algorithm-visualizer` (Dane will share the link):

```
algorithm-visualizer/
├── backend/        C++ backend — DO NOT TOUCH
├── docs/
│   ├── API.md      backend API contract — read it, don't edit it
│   └── FRONTEND_PROMPT.md   this prompt
├── frontend/       ← ALL frontend work goes here
│   ├── index.html
│   ├── style.css
│   └── js/
│       ├── mock-data.js
│       ├── api.js
│       ├── graph-view.js
│       ├── animation.js
│       └── main.js
└── CMakeLists.txt  C++ build — DO NOT TOUCH
```

`index.html` loads the scripts in this exact order (each file defines globals that the
later ones use):

```html
<script src="js/mock-data.js"></script>
<script src="js/api.js"></script>
<script src="js/graph-view.js"></script>
<script src="js/animation.js"></script>
<script src="js/main.js"></script>
```

### Team split

Four people work on four parts at the same time, each with their own AI assistant. Each
part must use **exactly** the names below (element ids, globals, function names,
attributes), or the parts won't fit together when we combine them. If you need something
another part provides, call it by the name listed here, and don't implement it yourself
beyond a temporary stub.

#### Part 1: Page layout and styling (`frontend/index.html`, `frontend/style.css`)

The page structure and all the CSS. `index.html` must contain these elements with
**exactly these ids**:

| id               | element    | purpose |
|------------------|------------|---------|
| `graph`          | `<svg>`    | where the graph is drawn |
| `source-select`  | `<select>` | source node |
| `target-select`  | `<select>` | target node |
| `run-dijkstra`   | `<button>` | run Dijkstra |
| `run-bfs`        | `<button>` | run BFS |
| `play`           | `<button>` | play / resume the animation |
| `pause`          | `<button>` | pause |
| `step`           | `<button>` | advance one step |
| `reset`          | `<button>` | reset the animation |
| `speed`          | `<input type="range">` | delay between steps in ms (min 50, max 1500, default 500) |
| `step-message`   | `<div>`    | the current step's `message` |
| `step-log`       | `<ol>`     | list of all steps played so far |
| `result-dijkstra`| `<div>`    | last Dijkstra result (path, distance, hops, stats) |
| `result-bfs`     | `<div>`    | last BFS result (same) |
| `error`          | `<div>`    | error messages (hidden when empty) |

Also add a small **legend** that explains the colours.

SVG styling is driven by `data-` attributes that Part 2 sets. Style these selectors in
`style.css`:

- `.node[data-state="default" | "frontier" | "current" | "visited" | "path"] circle`
- `.node[data-role="source"] circle`, `.node[data-role="target"] circle`. Use an outline
  or ring here, so the role stays visible alongside any state colour.
- `.edge[data-state="default" | "examining" | "relaxed" | "path"]`
- `.node-label` (the letter inside the circle), `.distance-label` (the distance shown next to the
  node), `.edge-weight` (the number on each edge)

Pick clear, colour-blind-friendly colours. The `path` state must stand out the most.

#### Part 2: Graph drawing (`frontend/js/graph-view.js`)

Defines one global object, `GraphView`:

```js
GraphView.render(svgElement, graph)          // draw nodes + edges from the /api/graph JSON
GraphView.reset()                             // every node/edge back to "default", clear distance labels
GraphView.setNodeState(nodeId, state)         // "default" | "frontier" | "current" | "visited" | "path"
GraphView.setNodeRole(nodeId, role)           // "source" | "target" | null  (independent of state)
GraphView.setEdgeState(from, to, state)       // "default" | "examining" | "relaxed" | "path"; undirected: (a,b) == (b,a)
GraphView.setDistanceLabel(nodeId, text)      // e.g. "∞", "7"; "" hides it
GraphView.highlightPath(pathArray)            // all nodes and edges along the path -> "path"
GraphView.onNodeClick(callback)               // callback(nodeId) when a node is clicked
```

SVG structure (Part 1's CSS depends on it):

```html
<g class="edge-group">
  <line class="edge" data-from="0" data-to="1" data-state="default" ... />
  <text class="edge-weight" ...>4</text>
</g>
<g class="node" data-id="0" data-state="default" data-role="">
  <circle r="22" ... />
  <text class="node-label">A</text>
  <text class="distance-label"></text>
</g>
```

Draw edges before nodes so the nodes sit on top. Put each edge's weight at the edge's
midpoint.

#### Part 3: Step animation (`frontend/js/animation.js`)

Defines one global object, `Animator`, which plays a result's `steps` using `GraphView`:

```js
Animator.load(result)          // stop any playback, GraphView.reset(), set distance label "∞" on
                               // every node and "0" on the source, mark source/target roles
Animator.play()
Animator.pause()
Animator.stepForward()         // play exactly one step
Animator.reset()               // back to before the first step (same as load again)
Animator.setSpeed(ms)          // delay between steps while playing
Animator.onStep(callback)      // callback(step, index, total) after each step is applied
Animator.onFinish(callback)    // callback(result) after the last step
```

Map each step type to visuals:

- `visit`: the previously "current" node becomes "visited". This node becomes "current".
  Set its distance label.
- `examine`: this edge becomes "examining". The previously examined edge goes back to
  whatever state it had before.
- `relax`: set `to`'s distance label to `distance`. If `to` isn't visited, make it
  "frontier". The edge becomes "relaxed". If an earlier relaxed edge led into the same
  `to` node, set that edge back to "default", because the route was improved.
- `stale`: no state change. The message is enough.
- `done`: the current node becomes "visited", then `GraphView.highlightPath(result.path)`.
- Unknown types: skip.

Use `setTimeout` for playback, not `requestAnimationFrame` timing tricks or animation
libraries. Pressing a Run button while an animation is playing must stop the old one
cleanly.

#### Part 4: Data, controls and results (`frontend/js/mock-data.js`, `frontend/js/api.js`, `frontend/js/main.js`)

`mock-data.js` defines three globals, copied exactly from the samples in `docs/API.md`:
`MOCK_GRAPH`, `MOCK_DIJKSTRA` (A → H), `MOCK_BFS` (A → H).

`api.js` defines:

```js
const USE_MOCK = true;     // switch to false once the backend is running
const API_BASE = "";       // "" when served by the backend; "http://localhost:18080" if opened as a file
async function fetchGraph()                          // -> graph JSON
async function runAlgorithm(name, source, target)    // name: "dijkstra" | "bfs" -> result JSON
```

- In mock mode, `runAlgorithm` returns `MOCK_DIJKSTRA` or `MOCK_BFS` for any source and target
  (fine for development).
- In real mode, it POSTs JSON and throws an `Error` carrying the backend's `error` message on
  HTTP 400.

`main.js` wires everything together on page load:

1. `fetchGraph()`, then `GraphView.render(...)`, and fill both selects with node labels
   (option values are ids). The defaults are source A and target H.
2. Clicking a node sets the source, and shift-clicking sets the target. Keep the selects
   in sync.
3. The Run buttons call `runAlgorithm`, then `Animator.load(result)` and `Animator.play()`.
4. The play/pause/step/reset buttons and the speed slider call the matching `Animator`
   functions.
5. `Animator.onStep`: show the step's `message` in `#step-message` and append it to `#step-log`.
6. `Animator.onFinish`: show the result in `#result-dijkstra` or `#result-bfs`. That's the
   path as labels joined with `→`, the distance, the hops and the `stats` fields (with
   readable names). If `found` is false, say "No path".
7. Show any error in `#error`.

### Working on your part before the others are done

Your part may need functions from parts that don't exist yet. In that case, create a
small **temporary stub** in a separate file you don't commit (for example
`frontend/js/stub.js`), or test with the mock data. Don't commit stubs over another
person's file.

### Git workflow

- Clone the repo, then create your own branch: `git checkout -b frontend-<yourname>`.
- Only commit files inside `frontend/`, and only the files for your part. That keeps
  merges conflict-free.
- Commit with clear messages (e.g. `Add graph SVG rendering`), push your branch, and open
  a pull request to the main branch on GitHub. Dane reviews and merges.
- Don't commit editor folders, OS files or the `build/` folder.

### Running it

- **Before the backend is ready:** with `USE_MOCK = true`, open `frontend/index.html`
  in the browser.
- **With the backend:** Dane builds and starts the server (`./build/server`). Set
  `USE_MOCK = false` and open `http://localhost:18080/`.

### What I want from you (the AI assistant)

1. Briefly restate my assigned part and the exact names (ids, globals, functions) I must
   use.
2. Write complete, working code for **my part only**, with short comments. Keep it
   readable. We have to explain it in a presentation.
3. Tell me how to test my part on its own (with mock data or stubs).
4. If anything in this prompt or `docs/API.md` is unclear or contradictory, ask me
   instead of guessing.

## END PROMPT

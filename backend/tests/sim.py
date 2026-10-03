import json, sys, heapq, urllib.request

B = "http://localhost:18080"
def get(path): return json.load(urllib.request.urlopen(B + path))
def post(path, body): return json.load(urllib.request.urlopen(urllib.request.Request(B + path, json.dumps(body).encode())))

graph = get("/api/graph")
L = {n["id"]: n["label"] for n in graph["nodes"]}
adj = {n["id"]: [] for n in graph["nodes"]}
for e in graph["edges"]:
    adj[e["from"]].append((e["to"], e["weight"])); adj[e["to"]].append((e["from"], e["weight"]))

class View:
    """State of the algorithm after `position` steps, rebuilt from scratch by replaying (like the frontend's Previous)."""
    def __init__(self, result, position):
        self.position, self.result = position, result
        n = len(L)
        self.dist = {i: None for i in range(n)}; self.via = {i: None for i in range(n)}
        self.status = {i: "unseen" for i in range(n)}
        s = result["source"]; self.dist[s] = 0; self.status[s] = "frontier"
        self.queue = [(0, s)]; self.current = None; self.step = None
        for step in result["steps"][:position]: self.apply(step)
    def apply(self, st):
        self.step = st; t = st["type"]
        if t in ("visit", "done") and self.current is not None:
            self.status[self.current] = "visited"; self.current = None
        if t == "visit":
            self.current = st["node"]; self.status[st["node"]] = "current"; self.dist[st["node"]] = st["distance"]
            self.queue.remove((st["distance"], st["node"]))
        elif t == "relax":
            n = st["to"]; self.dist[n], self.via[n] = st["distance"], st["from"]
            if self.status[n] == "unseen": self.status[n] = "frontier"
            self.queue.append((st["distance"], n))
        elif t == "stale": self.queue.remove((st["distance"], st["node"]))
    def ordered_queue(self):
        return sorted(self.queue) if self.result["algorithm"] == "dijkstra" else list(self.queue)
    def show(self, button):
        total = len(self.result["steps"])
        msg = self.step["message"] if self.step else "(press Next to start)"
        q = " ".join(f"{L[n]}:{d}" for d, n in self.ordered_queue()) or "empty"
        dist = "  ".join(f"{L[i]}={'∞' if self.dist[i] is None else self.dist[i]}" for i in sorted(L))
        print(f"[{button:^8}] step {self.position:2}/{total}  {msg}\n             dist: {dist}\n             queue: {q}")

def session(result, presses):
    print(f"\n===== {result['algorithm'].upper()} {L[result['source']]} -> {L[result['target']]}: scripted button presses =====")
    pos = 0; View(result, 0).show("start")
    for p in presses:
        if p == "next" and pos < len(result["steps"]): pos += 1
        elif p == "prev" and pos > 0: pos -= 1
        View(result, pos).show(p.upper())

dij = post("/api/dijkstra", {"source": 0, "target": 7})
bfs = post("/api/bfs", {"source": 0, "target": 7})
# first few presses, then walk into the stale step, press Previous twice, Next again, then jump to the end
session(dij, ["next"] * 8 + ["prev", "prev", "next"])

if len(sys.argv) > 1 and sys.argv[1] == "stale":
    print("\n===== DIJKSTRA: around the stale entry (jump to step 30, then presses) =====")
    for pos, btn in [(30, "goto 30"), (31, "NEXT"), (32, "NEXT"), (33, "NEXT"), (32, "PREV"), (33, "NEXT")]:
        View(dij, pos).show(btn)
    print("\n===== DIJKSTRA: last steps =====")
    for pos, btn in [(40, "goto 40"), (41, "NEXT"), (42, "NEXT"), (42, "NEXT (end)")]:
        View(dij, pos).show(btn)
    r = dij; print("\nresult box:", " -> ".join(L[n] for n in r["path"]), "| cost", r["distance"], "| edges", r["hops"], "|", {k: v for k, v in r["stats"].items() if k != "timeMicroseconds"})
    print("\n===== BFS A -> H: first presses and last =====")
    for pos, btn in [(0, "start"), (1, "NEXT"), (2, "NEXT"), (3, "NEXT"), (34, "goto 34"), (35, "NEXT"), (36, "NEXT")]:
        View(bfs, pos).show(btn)
    r = bfs; print("\nresult box:", " -> ".join(L[n] for n in r["path"]), "| cost", r["distance"], "| edges", r["hops"], "|", {k: v for k, v in r["stats"].items() if k != "timeMicroseconds"})

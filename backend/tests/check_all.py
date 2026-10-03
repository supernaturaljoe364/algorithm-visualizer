import heapq, random, copy, sys
sys.argv = ["x"]; exec(open(sys.argv[0] if False else __file__.replace("check_all.py", "sim.py")).read().split("dij = post(")[0])

def ref_dijkstra(s):
    d = {i: float("inf") for i in L}; d[s] = 0; q = [(0, s)]
    while q:
        x, u = heapq.heappop(q)
        if x > d[u]: continue
        for v, w in adj[u]:
            if x + w < d[v]: d[v] = x + w; heapq.heappush(q, (d[v], v))
    return d
def ref_hops(s):
    h = {s: 0}; q = [s]
    for u in q:
        for v, _ in adj[u]:
            if v not in h: h[v] = h[u] + 1; q.append(v)
    return h
def edge_w(a, b): return min(w for v, w in adj[a] if v == b)
def snap(v): return (v.dist, v.via, v.status, v.current, v.ordered_queue())

checks = positions = 0
def ok(cond, msg):
    global checks; checks += 1
    if not cond: print("FAIL:", msg); sys.exit(1)

for s in L:
    rd, rh = ref_dijkstra(s), ref_hops(s)
    for t in L:
        for algo in ("dijkstra", "bfs"):
            r = post("/api/" + algo, {"source": s, "target": t}); tag = f"{algo} {L[s]}->{L[t]}"
            p = r["path"]
            ok(r["found"] and p[0] == s and p[-1] == t, tag + " path endpoints")
            ok(all(any(v == b for v, _ in adj[a]) for a, b in zip(p, p[1:])), tag + " path uses real edges")
            cost = sum(edge_w(a, b) for a, b in zip(p, p[1:]))
            ok(r["distance"] == cost and r["hops"] == len(p) - 1, tag + " distance/hops match path")
            if algo == "dijkstra": ok(cost == rd[t], f"{tag} optimal cost {cost} vs {rd[t]}")
            else: ok(len(p) - 1 == rh[t], f"{tag} fewest edges")
            ok(cost >= rd[t], tag + " never cheaper than the optimum")
            st, stats = r["steps"], r["stats"]; count = lambda k: sum(x["type"] == k for x in st)
            ok(st[-1]["type"] == "done" and st[-1]["found"] and count("done") == 1, tag + " ends with done")
            ok(count("visit") == stats["nodesVisited"] and count("examine") == stats["edgesExamined"]
               and count("relax") == stats["edgesRelaxed"] and count("stale") == stats["staleEntriesSkipped"], tag + " stats match steps")
            ok(stats["queuePushes"] == 1 + stats["edgesRelaxed"] and stats["queuePops"] == stats["nodesVisited"] + stats["staleEntriesSkipped"], tag + " queue counters")
            # Next/Previous consistency at every position
            incremental = View(r, 0)
            for pos in range(len(st) + 1):
                positions += 1
                replayed = View(r, pos)
                ok(snap(replayed) == snap(incremental), f"{tag} replay == incremental at step {pos}")
                if pos > 0:  # Previous then Next must land on the same state
                    ok(snap(View(r, pos - 1)) != snap(replayed) or st[pos - 1]["type"] in ("examine", "done"), f"{tag} step {pos} changed state")
                if pos < len(st):
                    nxt = st[pos]
                    if nxt["type"] in ("visit", "stale"):  # pop must take the front of the queue
                        ok(replayed.ordered_queue()[0] == (nxt["distance"], nxt["node"]), f"{tag} pop is queue front at step {pos}")
                    incremental.apply(nxt)
            end = View(r, len(st))
            # final table: via-chain from the target reproduces the path; Dijkstra's visited distances are optimal
            chain, n = [t], t
            while end.via[n] is not None: n = end.via[n]; chain.append(n)
            ok(chain[::-1] == p, f"{tag} via-chain == path")
            if algo == "dijkstra":
                ok(all(end.dist[i] == rd[i] for i in L if end.status[i] == "visited"), tag + " visited nodes have final optimal distance")

print(f"ALL PASSED: {checks} checks, {positions} step positions replayed, 64 pairs x 2 algorithms over live HTTP")

// Talks to the C++ backend. Open index.html?mock to use the sample data instead.
const USE_MOCK = /[?&]mock\b/.test(location.search);
const API_BASE = location.port === "18080" ? "" : "http://localhost:18080";

async function fetchGraph() {
  if (USE_MOCK) return MOCK_GRAPH;
  const res = await fetch(API_BASE + "/api/graph");
  if (!res.ok) throw new Error("Could not load the graph (HTTP " + res.status + ")");
  return res.json();
}

// name: "dijkstra" | "bfs"
async function runAlgorithm(name, source, target) {
  if (USE_MOCK) return name === "bfs" ? MOCK_BFS : MOCK_DIJKSTRA; // always A -> H
  const res = await fetch(API_BASE + "/api/" + name, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ source: source, target: target })
  });
  const data = await res.json();
  if (!res.ok) throw new Error(data.error || "Request failed (HTTP " + res.status + ")");
  return data;
}

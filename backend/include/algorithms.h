#pragma once

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "graph.h"

inline constexpr int kInfinity = std::numeric_limits<int>::max();  // "no route found yet"

// One thing the algorithm did. The frontend steps through these.
//   "visit"   node, distance       took `node` out of the queue and processed it
//   "examine" from, to, weight     looked at the edge from -> to
//   "relax"   from, to, distance   found a better route to `to` (it goes on the queue)
//   "stale"   node, distance, best Dijkstra only: popped an outdated entry and skipped it
//   "done"    found                finished
// For BFS, `distance` counts edges instead of adding up weights.
struct Step {
    std::string type;
    int node = 0, from = 0, to = 0, weight = 0, distance = 0, best = 0;
    bool found = false;
};

struct Stats {
    int nodesVisited = 0, edgesExamined = 0, edgesRelaxed = 0;
    int queuePushes = 0, queuePops = 0, staleEntriesSkipped = 0;
    double timeMicroseconds = 0;
};

struct PathResult {
    std::string algorithm;
    int source = 0, target = 0;
    bool found = false;
    int distance = 0;        // total weight of `path` (only if found)
    std::vector<int> path{};   // source ... target (empty if not found)
    std::vector<Step> steps{};
    Stats stats{};
};

// Both throw std::out_of_range if source or target is not a node of the graph.
// recordSteps = false skips building `steps` (used by the benchmarks).
PathResult dijkstra(const Graph& g, int source, int target, bool recordSteps = true);
PathResult bfs(const Graph& g, int source, int target, bool recordSteps = true);

inline void checkEndpoints(const Graph& g, int source, int target) {
    if (source < 0 || source >= g.nodeCount()) throw std::out_of_range("source node " + std::to_string(source) + " does not exist");
    if (target < 0 || target >= g.nodeCount()) throw std::out_of_range("target node " + std::to_string(target) + " does not exist");
}

// Walks the predecessors back from the target to the source (the source has predecessor -1).
inline std::vector<int> buildPath(const std::vector<int>& prev, int target) {
    std::vector<int> path;
    for (int v = target; v != -1; v = prev[static_cast<size_t>(v)]) path.push_back(v);
    std::reverse(path.begin(), path.end());
    return path;
}

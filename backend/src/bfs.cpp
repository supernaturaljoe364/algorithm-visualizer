#include <chrono>
#include <queue>

#include "algorithms.h"

PathResult bfs(const Graph& g, int source, int target, bool recordSteps) {
    checkEndpoints(g, source, target);
    const auto startTime = std::chrono::steady_clock::now();

    PathResult result{.algorithm = "bfs", .source = source, .target = target};
    Stats& stats = result.stats;
    auto record = [&](Step step) {
        if (recordSteps) result.steps.push_back(std::move(step));
    };

    std::vector<int> edgesFromSource(g.nodes.size(), kInfinity);  // kInfinity = not discovered yet
    std::vector<int> cost(g.nodes.size(), 0);                     // total weight of the path BFS found
    std::vector<int> prev(g.nodes.size(), -1);

    std::queue<int> queue;  // plain first-in-first-out queue
    edgesFromSource[static_cast<size_t>(source)] = 0;
    queue.push(source);
    stats.queuePushes++;

    while (!queue.empty()) {
        const int u = queue.front();
        queue.pop();
        stats.queuePops++;
        stats.nodesVisited++;
        record({.type = "visit", .node = u, .distance = edgesFromSource[static_cast<size_t>(u)]});
        if (u == target) break;

        for (const Edge& edge : g.adjacent[static_cast<size_t>(u)]) {
            const auto v = static_cast<size_t>(edge.to);
            stats.edgesExamined++;
            record({.type = "examine", .from = u, .to = edge.to, .weight = edge.weight});

            // The first time BFS reaches a node is already the way with the fewest edges, so it
            // never updates a node twice. Weights play no part in this decision.
            if (edgesFromSource[v] == kInfinity) {
                edgesFromSource[v] = edgesFromSource[static_cast<size_t>(u)] + 1;
                cost[v] = cost[static_cast<size_t>(u)] + edge.weight;
                prev[v] = u;
                queue.push(edge.to);
                stats.queuePushes++;
                stats.edgesRelaxed++;
                record({.type = "relax", .from = u, .to = edge.to, .distance = edgesFromSource[v]});
            }
        }
    }

    result.found = edgesFromSource[static_cast<size_t>(target)] != kInfinity;
    if (result.found) {
        result.distance = cost[static_cast<size_t>(target)];
        result.path = buildPath(prev, target);
    }
    record({.type = "done", .found = result.found});

    stats.timeMicroseconds = std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - startTime).count();
    return result;
}

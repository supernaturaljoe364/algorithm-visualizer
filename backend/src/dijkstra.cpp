#include <chrono>
#include <functional>
#include <queue>
#include <utility>

#include "algorithms.h"

PathResult dijkstra(const Graph& g, int source, int target, bool recordSteps) {
    checkEndpoints(g, source, target);
    const auto startTime = std::chrono::steady_clock::now();

    PathResult result{.algorithm = "dijkstra", .source = source, .target = target};
    Stats& stats = result.stats;
    auto record = [&](Step step) {
        if (recordSteps) result.steps.push_back(std::move(step));
    };

    // 1. Distance initialization: every node is unreachable until a route is found.
    std::vector<int> dist(g.nodes.size(), kInfinity);
    std::vector<int> prev(g.nodes.size(), -1);  // the node we came from on the best known route

    // Min-heap of (distance, node). priority_queue is a max-heap by default, so we flip it with
    // std::greater. Pairs compare distance first, then node id, so ties go to the smaller id.
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> queue;

    // 2. Source initialization, and 3. the first insertion into the priority queue.
    dist[static_cast<size_t>(source)] = 0;
    queue.push({0, source});
    stats.queuePushes++;

    while (!queue.empty()) {
        // 4. Extract the entry with the smallest distance.
        const auto [d, u] = queue.top();
        queue.pop();
        stats.queuePops++;

        // 5. Stale entry: u was queued with distance d, but a shorter route was found later and u
        //    has already been processed with that shorter distance. Skip the outdated entry.
        if (d > dist[static_cast<size_t>(u)]) {
            stats.staleEntriesSkipped++;
            record({.type = "stale", .node = u, .distance = d, .best = dist[static_cast<size_t>(u)]});
            continue;
        }

        // u's distance is now final: every unfinished node is at least as far away.
        stats.nodesVisited++;
        record({.type = "visit", .node = u, .distance = d});
        if (u == target) break;

        // 6. Examine each neighbour of u.
        for (const Edge& edge : g.adjacent[static_cast<size_t>(u)]) {
            const auto v = static_cast<size_t>(edge.to);
            stats.edgesExamined++;
            record({.type = "examine", .from = u, .to = edge.to, .weight = edge.weight});

            // 7. Relaxation: is the route through u shorter than the best one known to v?
            if (d + edge.weight < dist[v]) {
                dist[v] = d + edge.weight;  // 8. update the distance
                prev[v] = u;                // 9. update the predecessor
                queue.push({dist[v], edge.to});  // the old entry for v stays in the heap and becomes stale
                stats.queuePushes++;
                stats.edgesRelaxed++;
                record({.type = "relax", .from = u, .to = edge.to, .distance = dist[v]});
            }
        }
    }

    const int best = dist[static_cast<size_t>(target)];
    result.found = best != kInfinity;
    if (result.found) {
        result.distance = best;
        result.path = buildPath(prev, target);  // 10. follow the predecessors back to the source
    }
    record({.type = "done", .found = result.found});

    stats.timeMicroseconds = std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - startTime).count();
    return result;
}

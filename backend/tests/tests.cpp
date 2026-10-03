#include <cstdio>
#include <vector>

#include "algorithms.h"
#include "demo_graph.h"
#include "random_graph.h"

// ---- A tiny test framework: TEST(name) { CHECK(condition); } ----
std::vector<std::pair<const char*, void (*)()>>& allTests() {
    static std::vector<std::pair<const char*, void (*)()>> tests;
    return tests;
}
int failures = 0;
#define TEST(name)                                             \
    static void name();                                        \
    static const int name##Registered = (allTests().push_back({#name, name}), 0); \
    static void name()
#define CHECK(condition)                                                       \
    do {                                                                       \
        if (!(condition)) { std::printf("  FAILED line %d: %s\n", __LINE__, #condition); ++failures; } \
    } while (0)

using Path = std::vector<int>;

Graph makeGraph(int nodeCount, std::vector<Edge> edges) {
    Graph g;
    for (int i = 0; i < nodeCount; ++i) g.addNode(std::string(1, static_cast<char>('A' + i)));
    for (const Edge& e : edges) g.addEdge(e.from, e.to, e.weight);
    return g;
}

// Slow but obviously correct reference (Bellman-Ford) to compare Dijkstra against.
int referenceDistance(const Graph& g, int source, int target) {
    std::vector<int> dist(g.nodes.size(), kInfinity);
    dist[static_cast<size_t>(source)] = 0;
    for (size_t round = 0; round < g.nodes.size(); ++round) {
        for (const Edge& e : g.edges) {
            auto a = static_cast<size_t>(e.from), b = static_cast<size_t>(e.to);
            if (dist[a] != kInfinity && dist[a] + e.weight < dist[b]) dist[b] = dist[a] + e.weight;
            if (dist[b] != kInfinity && dist[b] + e.weight < dist[a]) dist[a] = dist[b] + e.weight;
        }
    }
    return dist[static_cast<size_t>(target)];
}

// ---- Basic graphs ----
TEST(simple_graph) {
    Graph g = makeGraph(3, {{0, 1, 4}, {1, 2, 3}});
    PathResult r = dijkstra(g, 0, 2);
    CHECK(r.found && r.distance == 7 && r.path == (Path{0, 1, 2}));
    CHECK(bfs(g, 0, 2).path == (Path{0, 1, 2}));
}

TEST(multiple_paths_cheapest_wins) {  // the square from the project description
    Graph g = makeGraph(4, {{0, 1, 4}, {0, 2, 2}, {1, 3, 3}, {2, 3, 1}});
    PathResult r = dijkstra(g, 0, 3);
    CHECK(r.distance == 3 && r.path == (Path{0, 2, 3}));
}

TEST(bfs_and_dijkstra_differ_when_weights_matter) {
    // Direct edge: 1 edge but cost 10. Detour: 3 edges but cost 3.
    Graph g = makeGraph(4, {{0, 3, 10}, {0, 1, 1}, {1, 2, 1}, {2, 3, 1}});
    PathResult d = dijkstra(g, 0, 3), b = bfs(g, 0, 3);
    CHECK(d.path == (Path{0, 1, 2, 3}) && d.distance == 3);
    CHECK(b.path == (Path{0, 3}) && b.distance == 10);
}

TEST(demo_graph_a_to_h) {
    Graph g = makeDemoGraph();
    PathResult d = dijkstra(g, 0, 7), b = bfs(g, 0, 7);
    CHECK(d.path == (Path{0, 2, 4, 6, 7}) && d.distance == 10);  // A C E G H
    CHECK(b.path == (Path{0, 3, 7}) && b.distance == 16);        // A D H
    CHECK(b.path.size() < d.path.size() && d.distance < b.distance);
}

TEST(equal_cost_paths) {  // A-B-D and A-C-D both cost 2; either answer is right
    Graph g = makeGraph(4, {{0, 1, 1}, {0, 2, 1}, {1, 3, 1}, {2, 3, 1}});
    PathResult r = dijkstra(g, 0, 3);
    CHECK(r.found && r.distance == 2 && r.path.size() == 3);
}

// ---- Edge cases ----
TEST(unreachable_target) {
    Graph g = makeGraph(4, {{0, 1, 1}, {2, 3, 1}});  // two separate pieces
    for (PathResult r : {dijkstra(g, 0, 3), bfs(g, 0, 3)}) CHECK(!r.found && r.path.empty());
}

TEST(source_equals_target) {
    Graph g = makeDemoGraph();
    for (PathResult r : {dijkstra(g, 3, 3), bfs(g, 3, 3)}) CHECK(r.found && r.distance == 0 && r.path == (Path{3}));
}

TEST(single_node_graph) {
    Graph g = makeGraph(1, {});
    CHECK(dijkstra(g, 0, 0).found && bfs(g, 0, 0).found);
}

TEST(unknown_nodes_are_rejected) {
    Graph g = makeGraph(2, {{0, 1, 1}});
    int caught = 0;
    for (auto [s, t] : {std::pair{5, 0}, {0, 5}, {-1, 0}}) {
        try { dijkstra(g, s, t); } catch (const std::out_of_range&) { ++caught; }
        try { bfs(g, s, t); } catch (const std::out_of_range&) { ++caught; }
    }
    CHECK(caught == 6);
}

TEST(negative_weight_is_rejected) {
    Graph g = makeGraph(2, {});
    bool caught = false;
    try { g.addEdge(0, 1, -1); } catch (const std::invalid_argument&) { caught = true; }
    CHECK(caught);
}

// ---- Statistics and steps ----
TEST(dijkstra_stats_on_demo_graph) {  // hand-traced; the same numbers are in docs/API.md
    Stats s = dijkstra(makeDemoGraph(), 0, 7).stats;
    CHECK(s.nodesVisited == 8 && s.edgesExamined == 23 && s.edgesRelaxed == 9);
    CHECK(s.queuePushes == 10 && s.queuePops == 9 && s.staleEntriesSkipped == 1);
}

TEST(bfs_stats_on_demo_graph) {
    Stats s = bfs(makeDemoGraph(), 0, 7).stats;
    CHECK(s.nodesVisited == 7 && s.edgesExamined == 21 && s.edgesRelaxed == 7);
    CHECK(s.queuePushes == 8 && s.queuePops == 7 && s.staleEntriesSkipped == 0);
}

TEST(steps_describe_the_run) {
    Graph g = makeDemoGraph();
    PathResult r = dijkstra(g, 0, 7);
    CHECK(r.steps.size() == 42);
    CHECK(r.steps.front().type == "visit" && r.steps.front().node == 0);
    CHECK(r.steps.back().type == "done" && r.steps.back().found);

    int visits = 0, relaxes = 0, stale = 0;
    for (const Step& s : r.steps) {
        visits += s.type == "visit";
        relaxes += s.type == "relax";
        if (s.type == "stale") {  // D was queued at 7 via A, then improved to 6 via B
            ++stale;
            CHECK(s.node == 3 && s.distance == 7 && s.best == 6);
        }
    }
    CHECK(visits == r.stats.nodesVisited && relaxes == r.stats.edgesRelaxed && stale == 1);
    CHECK(bfs(g, 0, 7).steps.size() == 36);
}

TEST(equal_distances_are_taken_in_node_id_order) {  // the frontend's queue display relies on this
    Graph g = makeGraph(3, {{0, 2, 1}, {0, 1, 1}});
    PathResult r = dijkstra(g, 0, 2);
    std::vector<int> visited;
    for (const Step& s : r.steps) if (s.type == "visit") visited.push_back(s.node);
    CHECK(visited == (Path{0, 1, 2}));
}

TEST(steps_can_be_switched_off) {
    PathResult r = dijkstra(makeDemoGraph(), 0, 7, false);
    CHECK(r.steps.empty() && r.distance == 10 && r.stats.edgesExamined == 23);
}

// ---- A larger generated graph ----
TEST(random_graphs_match_the_reference) {
    for (unsigned seed = 1; seed <= 20; ++seed) {
        Graph g = randomGraph(300, 900, 25, seed);
        for (int target : {299, 150, 1}) {
            PathResult d = dijkstra(g, 0, target), b = bfs(g, 0, target);
            CHECK(d.found && d.distance == referenceDistance(g, 0, target));
            CHECK(b.found && d.distance <= b.distance);      // BFS is never cheaper
            CHECK(b.path.size() <= d.path.size());           // Dijkstra never uses fewer edges
        }
    }
}

int main() {
    for (const auto& [name, test] : allTests()) {
        const int before = failures;
        test();
        std::printf("[%s] %s\n", failures == before ? "PASS" : "FAIL", name);
    }
    std::printf("\n%zu tests, %d failed checks\n", allTests().size(), failures);
    return failures == 0 ? 0 : 1;
}

// Times Dijkstra and BFS and prints their operation counts. Run: ./build/benchmarks
//
// Each graph is a random connected graph (E = 4V) plus one isolated node used as the target.
// The target is unreachable, so both algorithms explore the whole graph: the worst case.
// Step recording is off, and the time shown is the median of 11 runs.

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include "algorithms.h"
#include "demo_graph.h"
#include "random_graph.h"

void run(const std::string& title, const Graph& g, int source, int target) {
    for (auto algorithm : {dijkstra, bfs}) {
        std::vector<double> times;
        PathResult r;
        for (int i = 0; i < 11; ++i) {
            r = algorithm(g, source, target, false);
            times.push_back(r.stats.timeMicroseconds);
        }
        std::sort(times.begin(), times.end());
        const Stats& s = r.stats;
        std::printf("%-9s %-9s %10.1f us | visited %7d  examined %8d  relaxed %7d  pushes %7d  pops %7d  stale %6d\n",
                    title.c_str(), r.algorithm.c_str(), times[5], s.nodesVisited, s.edgesExamined, s.edgesRelaxed,
                    s.queuePushes, s.queuePops, s.staleEntriesSkipped);
    }
}

int main() {
    std::printf("Demo graph, A to H\n");
    run("V=8", makeDemoGraph(), 0, 7);

    std::printf("\nRandom graphs, E = 4V, target unreachable (whole graph explored)\n");
    for (int v : {1000, 10000, 100000, 400000}) {
        Graph g = randomGraph(v, 3 * v + 1, 100, 1);  // v-1 tree edges + 3v+1 extra = 4v edges
        g.addNode("isolated");
        run("V=" + std::to_string(v), g, 0, v);
    }
}

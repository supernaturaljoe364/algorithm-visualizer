#pragma once

#include <random>
#include <string>

#include "graph.h"

// Connected random graph: a random tree (so every node is reachable) plus extra random edges.
// Same seed => same graph.
inline Graph randomGraph(int nodes, int extraEdges, int maxWeight, unsigned seed) {
    std::mt19937 rng(seed);
    auto below = [&](int n) { return static_cast<int>(rng() % static_cast<unsigned>(n)); };
    Graph g;
    for (int i = 0; i < nodes; ++i) g.addNode(std::to_string(i));
    for (int i = 1; i < nodes; ++i) g.addEdge(i, below(i), 1 + below(maxWeight));
    for (int i = 0; i < extraEdges; ++i) g.addEdge(below(nodes), below(nodes), 1 + below(maxWeight));
    return g;
}

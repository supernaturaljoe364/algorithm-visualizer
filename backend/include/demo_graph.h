#pragma once

#include <array>
#include <tuple>
#include <vector>

#include "graph.h"

// The graph shown in the frontend: A=0 B=1 C=2 D=3 E=4 F=5 G=6 H=7.
// From A to H the algorithms disagree:
//   BFS      (fewest edges): A -> D -> H            2 edges, cost 16
//   Dijkstra (lowest cost):  A -> C -> E -> G -> H  4 edges, cost 10
inline Graph makeDemoGraph() {
    Graph g;
    const std::vector<std::tuple<const char*, int, int>> nodes = {  // label, x, y
        {"A", 80, 250}, {"B", 240, 100}, {"C", 240, 400}, {"D", 420, 180},
        {"E", 420, 340}, {"F", 620, 450}, {"G", 590, 300}, {"H", 760, 250}};
    for (const auto& [label, x, y] : nodes) g.addNode(label, x, y);

    const std::vector<std::array<int, 3>> edges = {  // from, to, weight
        {0, 1, 4}, {0, 2, 2}, {0, 3, 7}, {1, 3, 2}, {1, 4, 6}, {2, 3, 5}, {2, 4, 3},
        {3, 6, 4}, {3, 7, 9}, {4, 5, 4}, {4, 6, 2}, {5, 7, 2}, {6, 7, 3}};
    for (const auto& [from, to, weight] : edges) g.addEdge(from, to, weight);
    return g;
}

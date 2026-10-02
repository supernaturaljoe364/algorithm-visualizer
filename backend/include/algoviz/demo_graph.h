#pragma once

#include "algoviz/graph.h"

namespace algoviz {

// The fixed graph served to the frontend.
//
// Nodes: A=0 B=1 C=2 D=3 E=4 F=5 G=6 H=7
//
// Chosen so that from A to H the two algorithms disagree:
//   BFS      (fewest edges): A -> D -> H            2 edges, cost 16
//   Dijkstra (lowest cost):  A -> C -> E -> G -> H  4 edges, cost 10
Graph makeDemoGraph();

}  // namespace algoviz

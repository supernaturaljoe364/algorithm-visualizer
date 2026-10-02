#include "algoviz/demo_graph.h"

namespace algoviz {

Graph makeDemoGraph() {
    Graph g;
    for (const char* label : {"A", "B", "C", "D", "E", "F", "G", "H"}) {
        g.addNode(label);
    }

    enum : NodeId { A, B, C, D, E, F, G, H };
    g.addEdge(A, B, 4);
    g.addEdge(A, C, 2);
    g.addEdge(A, D, 7);
    g.addEdge(B, D, 2);
    g.addEdge(B, E, 6);
    g.addEdge(C, D, 5);
    g.addEdge(C, E, 3);
    g.addEdge(D, G, 4);
    g.addEdge(D, H, 9);
    g.addEdge(E, F, 4);
    g.addEdge(E, G, 2);
    g.addEdge(F, H, 2);
    g.addEdge(G, H, 3);
    return g;
}

}  // namespace algoviz

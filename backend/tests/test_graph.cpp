#include <stdexcept>

#include "algoviz/demo_graph.h"
#include "algoviz/graph.h"
#include "test_framework.h"

using namespace algoviz;

TEST(graph_starts_empty) {
    Graph g;
    CHECK_EQ(g.nodeCount(), 0u);
    CHECK_EQ(g.edgeCount(), 0u);
    CHECK(!g.hasNode(0));
}

TEST(graph_add_node_assigns_sequential_ids) {
    Graph g;
    CHECK_EQ(g.addNode("A"), 0u);
    CHECK_EQ(g.addNode("B"), 1u);
    CHECK_EQ(g.addNode("C"), 2u);
    CHECK_EQ(g.nodeCount(), 3u);
    CHECK_EQ(g.label(1), std::string("B"));
}

TEST(graph_edges_are_undirected) {
    Graph g;
    const NodeId a = g.addNode("A");
    const NodeId b = g.addNode("B");
    g.addEdge(a, b, 4);

    CHECK_EQ(g.neighbors(a).size(), 1u);
    CHECK_EQ(g.neighbors(a)[0].to, b);
    CHECK_EQ(g.neighbors(a)[0].weight, 4);

    CHECK_EQ(g.neighbors(b).size(), 1u);
    CHECK_EQ(g.neighbors(b)[0].to, a);
    CHECK_EQ(g.neighbors(b)[0].weight, 4);
}

TEST(graph_edge_list_stores_each_edge_once) {
    Graph g;
    g.addNode("A");
    g.addNode("B");
    g.addNode("C");
    g.addEdge(0, 1, 4);
    g.addEdge(1, 2, 3);

    CHECK_EQ(g.edgeCount(), 2u);
    CHECK_EQ(g.edges()[1].from, 1u);
    CHECK_EQ(g.edges()[1].to, 2u);
    CHECK_EQ(g.edges()[1].weight, 3);
}

TEST(graph_isolated_node_has_no_neighbors) {
    Graph g;
    g.addNode("A");
    g.addNode("B");
    CHECK(g.neighbors(1).empty());
}

TEST(graph_zero_weight_edge_is_allowed) {
    Graph g;
    g.addNode("A");
    g.addNode("B");
    g.addEdge(0, 1, 0);
    CHECK_EQ(g.neighbors(0)[0].weight, 0);
}

TEST(graph_rejects_negative_weight) {
    Graph g;
    g.addNode("A");
    g.addNode("B");
    CHECK_THROWS(g.addEdge(0, 1, -1), std::invalid_argument);
    CHECK_EQ(g.edgeCount(), 0u);
}

TEST(graph_rejects_unknown_nodes) {
    Graph g;
    g.addNode("A");
    CHECK_THROWS(g.addEdge(0, 5, 1), std::out_of_range);
    CHECK_THROWS(g.neighbors(5), std::out_of_range);
    CHECK_THROWS(g.label(5), std::out_of_range);
}

TEST(demo_graph_has_expected_shape) {
    const Graph g = makeDemoGraph();
    CHECK_EQ(g.nodeCount(), 8u);
    CHECK_EQ(g.edgeCount(), 13u);
    CHECK_EQ(g.label(0), std::string("A"));
    CHECK_EQ(g.label(7), std::string("H"));

    // Every undirected edge appears in exactly two adjacency lists.
    std::size_t adjacencyEntries = 0;
    for (NodeId u = 0; u < g.nodeCount(); ++u) {
        adjacencyEntries += g.neighbors(u).size();
    }
    CHECK_EQ(adjacencyEntries, 2 * g.edgeCount());
}

#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace algoviz {

using NodeId = std::size_t;
using Weight = int;

// One entry in a node's adjacency list: "there is an edge to `to` costing `weight`".
struct AdjacentEdge {
    NodeId to;
    Weight weight;
};

// An edge as it was added to the graph. Used for listing the graph (e.g. GET /api/graph).
struct Edge {
    NodeId from;
    NodeId to;
    Weight weight;
};

// Undirected, weighted graph stored as an adjacency list.
// Nodes are numbered 0..nodeCount()-1 in the order they are added.
class Graph {
public:
    NodeId addNode(std::string label);

    // Adds an undirected edge. Throws std::out_of_range for unknown nodes and
    // std::invalid_argument for negative weights (Dijkstra requires weights >= 0).
    void addEdge(NodeId from, NodeId to, Weight weight);

    std::size_t nodeCount() const { return adjacency_.size(); }
    std::size_t edgeCount() const { return edges_.size(); }
    bool hasNode(NodeId id) const { return id < nodeCount(); }

    const std::vector<AdjacentEdge>& neighbors(NodeId id) const;
    const std::string& label(NodeId id) const;
    const std::vector<Edge>& edges() const { return edges_; }

private:
    void requireNode(NodeId id) const;

    std::vector<std::vector<AdjacentEdge>> adjacency_;  // adjacency_[u] = edges leaving u
    std::vector<std::string> labels_;
    std::vector<Edge> edges_;  // each undirected edge stored once
};

}  // namespace algoviz

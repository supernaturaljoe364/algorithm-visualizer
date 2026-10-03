#pragma once

#include <stdexcept>
#include <string>
#include <vector>

struct Edge {
    int from, to, weight;
};

struct Node {
    std::string label;
    int x, y;  // where the frontend draws the node
};

// Undirected weighted graph stored as an adjacency list. Nodes are numbered 0, 1, 2, ...
struct Graph {
    std::vector<Node> nodes;
    std::vector<std::vector<Edge>> adjacent;  // adjacent[u] = edges leaving u (each edge is stored in both directions)
    std::vector<Edge> edges;                  // every edge once, used to describe the graph in the API

    int nodeCount() const { return static_cast<int>(nodes.size()); }

    void addNode(std::string label, int x = 0, int y = 0) {
        nodes.push_back({std::move(label), x, y});
        adjacent.emplace_back();
    }

    void addEdge(int a, int b, int weight) {
        if (weight < 0) throw std::invalid_argument("edge weights must not be negative");
        auto& fromA = adjacent.at(static_cast<size_t>(a));  // at() throws std::out_of_range for unknown nodes
        auto& fromB = adjacent.at(static_cast<size_t>(b));
        fromA.push_back({a, b, weight});
        fromB.push_back({b, a, weight});
        edges.push_back({a, b, weight});
    }
};

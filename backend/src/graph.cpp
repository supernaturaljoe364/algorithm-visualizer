#include "algoviz/graph.h"

#include <stdexcept>
#include <utility>

namespace algoviz {

NodeId Graph::addNode(std::string label) {
    const NodeId id = nodeCount();
    adjacency_.emplace_back();
    labels_.push_back(std::move(label));
    return id;
}

void Graph::addEdge(NodeId from, NodeId to, Weight weight) {
    requireNode(from);
    requireNode(to);
    if (weight < 0) {
        throw std::invalid_argument("edge weight must be non-negative");
    }

    // Undirected: the edge can be traversed in both directions.
    adjacency_[from].push_back({to, weight});
    adjacency_[to].push_back({from, weight});
    edges_.push_back({from, to, weight});
}

const std::vector<AdjacentEdge>& Graph::neighbors(NodeId id) const {
    requireNode(id);
    return adjacency_[id];
}

const std::string& Graph::label(NodeId id) const {
    requireNode(id);
    return labels_[id];
}

void Graph::requireNode(NodeId id) const {
    if (!hasNode(id)) {
        throw std::out_of_range("node id " + std::to_string(id) + " does not exist");
    }
}

}  // namespace algoviz

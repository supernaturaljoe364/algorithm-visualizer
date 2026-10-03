// HTTP layer: read JSON request -> run algorithm -> write JSON response.
// All the algorithm logic lives in dijkstra.cpp and bfs.cpp, which know nothing about HTTP.

#include "algorithms.h"
#include "crow.h"
#include "crow/middlewares/cors.h"
#include "demo_graph.h"

// Text the frontend shows for one step, using the node labels.
std::string describe(const Graph& g, const PathResult& r, const Step& s) {
    const bool isBfs = r.algorithm == "bfs";
    auto name = [&](int id) { return g.nodes[static_cast<size_t>(id)].label; };
    auto num = [](int n) { return std::to_string(n); };
    auto edges = [&](int n) { return num(n) + (n == 1 ? " edge" : " edges"); };

    if (s.type == "visit") return "Visit " + name(s.node) + (isBfs ? " (" + edges(s.distance) + " from source)" : " (distance " + num(s.distance) + ")");
    if (s.type == "examine") return "Examine edge " + name(s.from) + " -> " + name(s.to) + (isBfs ? "" : " (weight " + num(s.weight) + ")");
    if (s.type == "stale") return "Skip stale queue entry for " + name(s.node) + " (" + num(s.distance) + " > " + num(s.best) + ")";
    if (s.type == "relax") {
        if (isBfs) return "Discover " + name(s.to) + ": " + edges(s.distance) + " via " + name(s.from);
        return "Update " + name(s.to) + ": distance " + num(s.distance) + " via " + name(s.from);
    }
    if (!s.found) return "No path from " + name(r.source) + " to " + name(r.target);
    if (isBfs) return "Reached " + name(r.target) + " in " + edges(static_cast<int>(r.path.size()) - 1) + " (path cost " + num(r.distance) + ")";
    return "Reached " + name(r.target) + ": shortest distance " + num(r.distance);
}

crow::json::wvalue stepToJson(const Graph& g, const PathResult& r, const Step& s) {
    crow::json::wvalue json;
    json["type"] = s.type;
    if (s.type == "visit" || s.type == "stale") { json["node"] = s.node; json["distance"] = s.distance; }
    if (s.type == "examine") { json["from"] = s.from; json["to"] = s.to; json["weight"] = s.weight; }
    if (s.type == "relax") { json["from"] = s.from; json["to"] = s.to; json["distance"] = s.distance; }
    if (s.type == "done") json["found"] = s.found;
    json["message"] = describe(g, r, s);
    return json;
}

crow::json::wvalue resultToJson(const Graph& g, const PathResult& r) {
    crow::json::wvalue json;
    json["algorithm"] = r.algorithm;
    json["source"] = r.source;
    json["target"] = r.target;
    json["found"] = r.found;
    json["path"] = r.path;
    json["distance"] = r.found ? crow::json::wvalue(r.distance) : crow::json::wvalue(nullptr);
    json["hops"] = r.found ? crow::json::wvalue(static_cast<int>(r.path.size()) - 1) : crow::json::wvalue(nullptr);

    std::vector<crow::json::wvalue> steps;
    for (const Step& step : r.steps) steps.push_back(stepToJson(g, r, step));
    json["steps"] = std::move(steps);

    const Stats& s = r.stats;
    json["stats"] = {{"nodesVisited", s.nodesVisited},         {"edgesExamined", s.edgesExamined},
                     {"edgesRelaxed", s.edgesRelaxed},         {"queuePushes", s.queuePushes},
                     {"queuePops", s.queuePops},               {"staleEntriesSkipped", s.staleEntriesSkipped},
                     {"timeMicroseconds", s.timeMicroseconds}};
    return json;
}

crow::json::wvalue graphToJson(const Graph& g) {
    crow::json::wvalue json;
    json["width"] = 840;
    json["height"] = 500;
    std::vector<crow::json::wvalue> nodes, edges;
    for (size_t i = 0; i < g.nodes.size(); ++i) {
        nodes.push_back({{"id", static_cast<int>(i)}, {"label", g.nodes[i].label}, {"x", g.nodes[i].x}, {"y", g.nodes[i].y}});
    }
    for (const Edge& e : g.edges) edges.push_back({{"from", e.from}, {"to", e.to}, {"weight", e.weight}});
    json["nodes"] = std::move(nodes);
    json["edges"] = std::move(edges);
    return json;
}

// Handles POST {"source": 0, "target": 7}. Anything wrong with the request becomes HTTP 400.
crow::response handle(const Graph& g, const crow::request& request, PathResult (*algorithm)(const Graph&, int, int, bool)) {
    try {
        const auto body = crow::json::load(request.body);
        if (!body || !body.has("source") || !body.has("target")) throw std::invalid_argument("send JSON like {\"source\": 0, \"target\": 7}");
        auto nodeId = [&](const char* key) {  // Crow's .i() would silently turn "a" or 1.5 into a number
            const auto& value = body[key];
            if (value.t() != crow::json::type::Number || value.d() != static_cast<double>(value.i())) {
                throw std::invalid_argument(std::string("'") + key + "' must be a whole number");
            }
            return static_cast<int>(value.i());
        };
        return crow::response(resultToJson(g, algorithm(g, nodeId("source"), nodeId("target"), true)));
    } catch (const std::exception& e) {
        return crow::response(400, crow::json::wvalue({{"error", e.what()}}));
    }
}

int main() {
    const Graph graph = makeDemoGraph();  // read-only, so the server's threads can share it

    crow::App<crow::CORSHandler> app;
    app.get_middleware<crow::CORSHandler>().global().origin("*").headers("Content-Type");

    CROW_ROUTE(app, "/api/graph")([&] { return graphToJson(graph); });
    CROW_ROUTE(app, "/api/dijkstra").methods("POST"_method)([&](const crow::request& request) { return handle(graph, request, dijkstra); });
    CROW_ROUTE(app, "/api/bfs").methods("POST"_method)([&](const crow::request& request) { return handle(graph, request, bfs); });

    // Crow serves the other files from frontend/ (see CROW_STATIC_DIRECTORY in CMakeLists.txt).
    CROW_ROUTE(app, "/")([](const crow::request&, crow::response& response) {
        response.set_static_file_info_unsafe(CROW_STATIC_DIRECTORY "index.html");  // a constant path, nothing user-supplied
        response.end();
    });

    app.port(18080).multithreaded().run();
}

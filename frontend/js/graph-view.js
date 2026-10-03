// Draws the graph as inline SVG and changes node/edge colours via data- attributes.
const GraphView = (function () {
  const NS = "http://www.w3.org/2000/svg";
  let nodes = {}, edges = {}, clickCb = null;
  const key = (a, b) => (a < b ? a + "-" + b : b + "-" + a); // undirected edge key

  function make(tag, attrs, parent) {
    const e = document.createElementNS(NS, tag);
    for (const k in attrs) e.setAttribute(k, attrs[k]);
    if (parent) parent.appendChild(e);
    return e;
  }

  function render(svg, graph) {
    svg.innerHTML = "";
    nodes = {}; edges = {};
    svg.setAttribute("viewBox", "0 0 " + graph.width + " " + graph.height);
    const pos = {};
    graph.nodes.forEach(n => (pos[n.id] = n));

    graph.edges.forEach(e => { // edges first, so nodes sit on top
      const a = pos[e.from], b = pos[e.to];
      const g = make("g", { class: "edge-group" }, svg);
      edges[key(e.from, e.to)] = make("line", { class: "edge", "data-from": e.from, "data-to": e.to,
        "data-state": "default", x1: a.x, y1: a.y, x2: b.x, y2: b.y }, g);
      make("text", { class: "edge-weight", x: (a.x + b.x) / 2, y: (a.y + b.y) / 2 }, g).textContent = e.weight;
    });

    graph.nodes.forEach(n => {
      const g = make("g", { class: "node", "data-id": n.id, "data-state": "default", "data-role": "" }, svg);
      make("circle", { cx: n.x, cy: n.y, r: 22 }, g);
      make("text", { class: "node-label", x: n.x, y: n.y, dy: "0.35em" }, g).textContent = n.label;
      make("text", { class: "distance-label", x: n.x, y: n.y - 32 }, g);
      g.addEventListener("click", ev => clickCb && clickCb(n.id, ev));
      nodes[n.id] = g;
    });
  }

  const setNodeState = (id, s) => nodes[id] && nodes[id].setAttribute("data-state", s);
  const setNodeRole = (id, r) => nodes[id] && nodes[id].setAttribute("data-role", r || "");
  const setEdgeState = (a, b, s) => edges[key(a, b)] && edges[key(a, b)].setAttribute("data-state", s);
  const setDistanceLabel = (id, t) => nodes[id] && (nodes[id].querySelector(".distance-label").textContent = t);

  function reset() {
    Object.keys(nodes).forEach(id => { setNodeState(id, "default"); setDistanceLabel(id, ""); });
    Object.values(edges).forEach(e => e.setAttribute("data-state", "default"));
  }

  function highlightPath(path) {
    path.forEach((id, i) => {
      setNodeState(id, "path");
      if (i > 0) setEdgeState(path[i - 1], id, "path");
    });
  }

  return {
    render, reset, setNodeState, setNodeRole, setEdgeState, setDistanceLabel, highlightPath,
    nodeIds: () => Object.keys(nodes).map(Number), // extra helper used by Animator and main
    onNodeClick: cb => (clickCb = cb)               // cb(nodeId, clickEvent)
  };
})();

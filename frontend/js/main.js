// Connects the page: loads the graph, wires buttons, shows messages and results.
(async function () {
  const $ = id => document.getElementById(id);
  const labels = {};
  const STAT_NAMES = {
    nodesVisited: "Nodes visited", edgesExamined: "Edges examined", edgesRelaxed: "Edges relaxed",
    queuePushes: "Queue pushes", queuePops: "Queue pops", staleEntriesSkipped: "Stale entries skipped",
    timeMicroseconds: "Run time (µs)"
  };

  function showError(msg) { $("error").textContent = msg || ""; $("error").hidden = !msg; }

  function markRoles() { // show chosen source/target before running
    GraphView.nodeIds().forEach(id => GraphView.setNodeRole(id, null));
    GraphView.setNodeRole(+$("target-select").value, "target");
    GraphView.setNodeRole(+$("source-select").value, "source");
  }

  function showResult(r) {
    const name = r.algorithm === "bfs" ? "BFS" : "Dijkstra";
    let html = "<h3>" + name + " (" + labels[r.source] + " to " + labels[r.target] + ")</h3>";
    if (!r.found) html += "<p class='path'>No path</p>";
    else html += "<p class='path'>" + r.path.map(i => labels[i]).join(" → ") + "</p>" +
      "<p><b>Total distance:</b> " + r.distance + " &nbsp; <b>Edges:</b> " + r.hops + "</p>";
    html += "<table>";
    for (const k in r.stats) html += "<tr><td>" + (STAT_NAMES[k] || k) + "</td><td>" + r.stats[k] + "</td></tr>";
    $("result-" + r.algorithm).innerHTML = html + "</table>";
  }

  async function run(name) {
    showError("");
    try {
      const r = await runAlgorithm(name, +$("source-select").value, +$("target-select").value);
      Animator.load(r);
      Animator.play();
    } catch (e) {
      showError(e instanceof TypeError
        ? "Cannot reach the backend at localhost:18080. Start ./build/server, or open index.html?mock."
        : e.message);
    }
  }

  // Setup
  try {
    const g = await fetchGraph();
    GraphView.render($("graph"), g);
    g.nodes.forEach(n => {
      labels[n.id] = n.label;
      ["source-select", "target-select"].forEach(id => {
        const o = document.createElement("option");
        o.value = n.id; o.textContent = n.label;
        $(id).appendChild(o);
      });
    });
    $("source-select").value = g.nodes[0].id;
    $("target-select").value = g.nodes[g.nodes.length - 1].id;
    markRoles();
  } catch (e) {
    showError(e instanceof TypeError ? "Cannot reach the backend at localhost:18080. Start ./build/server, or open index.html?mock." : e.message);
    return;
  }

  GraphView.onNodeClick((id, ev) => { // click = source, shift-click = target
    $(ev.shiftKey ? "target-select" : "source-select").value = id;
    markRoles();
  });
  $("source-select").onchange = $("target-select").onchange = markRoles;

  $("run-dijkstra").onclick = () => run("dijkstra");
  $("run-bfs").onclick = () => run("bfs");
  $("play").onclick = Animator.play;
  $("pause").onclick = Animator.pause;
  $("prev").onclick = Animator.stepBack;
  $("step").onclick = Animator.stepForward;
  $("reset").onclick = Animator.reset;
  $("speed").oninput = () => { Animator.setSpeed(+$("speed").value); $("speed-value").textContent = $("speed").value + " ms"; };
  Animator.setSpeed(+$("speed").value);

  Animator.onStep(s => {
    $("step-message").textContent = s.message || s.type;
    const li = document.createElement("li");
    li.textContent = s.message || s.type;
    $("step-log").appendChild(li);
    li.scrollIntoView({ block: "nearest" });
  });
  Animator.onBack(s => {
    if ($("step-log").lastChild) $("step-log").removeChild($("step-log").lastChild);
    $("step-message").textContent = s ? s.message : "";
  });
  Animator.onReset(() => { $("step-log").innerHTML = ""; $("step-message").textContent = ""; });
  Animator.onFinish(showResult);
})();

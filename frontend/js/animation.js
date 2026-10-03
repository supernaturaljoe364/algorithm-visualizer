// Plays a result's steps on the graph. Step back = reset + silently replay (no undo logic).
const Animator = (function () {
  let result = null, idx = 0, playing = false, timer = null, delay = 500;
  let current, examEdge, examPrev, edgeState, relaxedFrom, visited;
  let stepCb = () => {}, finishCb = () => {}, backCb = () => {}, resetCb = () => {};
  const key = (a, b) => (a < b ? a + "-" + b : b + "-" + a);
  const total = () => (result ? result.steps.length : 0);

  function restoreExam() { // un-highlight the edge that was being examined
    if (examEdge) GraphView.setEdgeState(examEdge[0], examEdge[1], examPrev);
    examEdge = null;
  }

  function setEdge(a, b, s) { edgeState[key(a, b)] = s; GraphView.setEdgeState(a, b, s); }

  function apply(s) {
    if (s.type === "visit") {
      restoreExam();
      if (current !== null) { GraphView.setNodeState(current, "visited"); visited[current] = true; }
      current = s.node;
      GraphView.setNodeState(s.node, "current");
      GraphView.setDistanceLabel(s.node, String(s.distance));
    } else if (s.type === "examine") {
      restoreExam();
      examEdge = [s.from, s.to];
      examPrev = edgeState[key(s.from, s.to)] || "default";
      GraphView.setEdgeState(s.from, s.to, "examining");
    } else if (s.type === "relax") {
      examEdge = null; // this edge becomes "relaxed" below
      const old = relaxedFrom[s.to];
      if (old !== undefined && old !== s.from) setEdge(old, s.to, "default"); // route improved
      relaxedFrom[s.to] = s.from;
      setEdge(s.from, s.to, "relaxed");
      GraphView.setDistanceLabel(s.to, String(s.distance));
      if (!visited[s.to] && s.to !== current) GraphView.setNodeState(s.to, "frontier");
    } else if (s.type === "done") {
      restoreExam();
      if (current !== null) GraphView.setNodeState(current, "visited");
      GraphView.highlightPath(result.path);
    } // "stale" and unknown types: nothing to draw
  }

  // Put the graph in its starting look, then silently apply the first n steps.
  function rebuild(n) {
    current = null; examEdge = null; edgeState = {}; relaxedFrom = {}; visited = {};
    GraphView.reset();
    GraphView.nodeIds().forEach(id => { GraphView.setNodeRole(id, null); GraphView.setDistanceLabel(id, "∞"); });
    GraphView.setNodeRole(result.target, "target");
    GraphView.setNodeRole(result.source, "source");
    GraphView.setDistanceLabel(result.source, "0");
    for (let i = 0; i < n; i++) apply(result.steps[i]);
    idx = n;
  }

  function stepForward() {
    if (!result || idx >= total()) return false;
    const s = result.steps[idx];
    apply(s);
    idx++;
    stepCb(s, idx - 1, total());
    if (idx === total()) finishCb(result);
    return true;
  }

  function run() {
    if (!playing) return;
    if (!stepForward() || idx >= total()) { playing = false; return; }
    timer = setTimeout(run, delay);
  }

  function pause() { playing = false; clearTimeout(timer); }
  function load(r) { pause(); result = r; rebuild(0); resetCb(); }
  function reset() { if (result) { pause(); rebuild(0); resetCb(); } }
  function play() {
    if (!result || playing) return;
    if (idx >= total()) { rebuild(0); resetCb(); } // replay from the start
    playing = true;
    run();
  }
  function stepBack() {
    pause();
    if (!result || idx === 0) return;
    rebuild(idx - 1);
    backCb(idx > 0 ? result.steps[idx - 1] : null, idx, total());
  }

  return {
    load, play, pause, reset, stepForward, stepBack,
    setSpeed: ms => (delay = ms),
    onStep: cb => (stepCb = cb), onFinish: cb => (finishCb = cb),
    onBack: cb => (backCb = cb), onReset: cb => (resetCb = cb)
  };
})();

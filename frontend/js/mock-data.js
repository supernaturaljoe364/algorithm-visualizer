// Sample responses copied from docs/API.md. Used only when the page is opened with ?mock
const MOCK_GRAPH = {
 "width": 840,
 "height": 500,
 "nodes": [
  {
   "id": 0,
   "label": "A",
   "x": 80,
   "y": 250
  },
  {
   "id": 1,
   "label": "B",
   "x": 240,
   "y": 100
  },
  {
   "id": 2,
   "label": "C",
   "x": 240,
   "y": 400
  },
  {
   "id": 3,
   "label": "D",
   "x": 420,
   "y": 180
  },
  {
   "id": 4,
   "label": "E",
   "x": 420,
   "y": 340
  },
  {
   "id": 5,
   "label": "F",
   "x": 620,
   "y": 450
  },
  {
   "id": 6,
   "label": "G",
   "x": 590,
   "y": 300
  },
  {
   "id": 7,
   "label": "H",
   "x": 760,
   "y": 250
  }
 ],
 "edges": [
  {
   "from": 0,
   "to": 1,
   "weight": 4
  },
  {
   "from": 0,
   "to": 2,
   "weight": 2
  },
  {
   "from": 0,
   "to": 3,
   "weight": 7
  },
  {
   "from": 1,
   "to": 3,
   "weight": 2
  },
  {
   "from": 1,
   "to": 4,
   "weight": 6
  },
  {
   "from": 2,
   "to": 3,
   "weight": 5
  },
  {
   "from": 2,
   "to": 4,
   "weight": 3
  },
  {
   "from": 3,
   "to": 6,
   "weight": 4
  },
  {
   "from": 3,
   "to": 7,
   "weight": 9
  },
  {
   "from": 4,
   "to": 5,
   "weight": 4
  },
  {
   "from": 4,
   "to": 6,
   "weight": 2
  },
  {
   "from": 5,
   "to": 7,
   "weight": 2
  },
  {
   "from": 6,
   "to": 7,
   "weight": 3
  }
 ]
};
const MOCK_DIJKSTRA = {
 "algorithm": "dijkstra",
 "source": 0,
 "target": 7,
 "found": true,
 "path": [
  0,
  2,
  4,
  6,
  7
 ],
 "distance": 10,
 "hops": 4,
 "steps": [
  {
   "type": "visit",
   "node": 0,
   "distance": 0,
   "message": "Visit A (distance 0)"
  },
  {
   "type": "examine",
   "from": 0,
   "to": 1,
   "weight": 4,
   "message": "Examine edge A -> B (weight 4)"
  },
  {
   "type": "relax",
   "from": 0,
   "to": 1,
   "distance": 4,
   "message": "Update B: distance 4 via A"
  },
  {
   "type": "examine",
   "from": 0,
   "to": 2,
   "weight": 2,
   "message": "Examine edge A -> C (weight 2)"
  },
  {
   "type": "relax",
   "from": 0,
   "to": 2,
   "distance": 2,
   "message": "Update C: distance 2 via A"
  },
  {
   "type": "examine",
   "from": 0,
   "to": 3,
   "weight": 7,
   "message": "Examine edge A -> D (weight 7)"
  },
  {
   "type": "relax",
   "from": 0,
   "to": 3,
   "distance": 7,
   "message": "Update D: distance 7 via A"
  },
  {
   "type": "visit",
   "node": 2,
   "distance": 2,
   "message": "Visit C (distance 2)"
  },
  {
   "type": "examine",
   "from": 2,
   "to": 0,
   "weight": 2,
   "message": "Examine edge C -> A (weight 2)"
  },
  {
   "type": "examine",
   "from": 2,
   "to": 3,
   "weight": 5,
   "message": "Examine edge C -> D (weight 5)"
  },
  {
   "type": "examine",
   "from": 2,
   "to": 4,
   "weight": 3,
   "message": "Examine edge C -> E (weight 3)"
  },
  {
   "type": "relax",
   "from": 2,
   "to": 4,
   "distance": 5,
   "message": "Update E: distance 5 via C"
  },
  {
   "type": "visit",
   "node": 1,
   "distance": 4,
   "message": "Visit B (distance 4)"
  },
  {
   "type": "examine",
   "from": 1,
   "to": 0,
   "weight": 4,
   "message": "Examine edge B -> A (weight 4)"
  },
  {
   "type": "examine",
   "from": 1,
   "to": 3,
   "weight": 2,
   "message": "Examine edge B -> D (weight 2)"
  },
  {
   "type": "relax",
   "from": 1,
   "to": 3,
   "distance": 6,
   "message": "Update D: distance 6 via B"
  },
  {
   "type": "examine",
   "from": 1,
   "to": 4,
   "weight": 6,
   "message": "Examine edge B -> E (weight 6)"
  },
  {
   "type": "visit",
   "node": 4,
   "distance": 5,
   "message": "Visit E (distance 5)"
  },
  {
   "type": "examine",
   "from": 4,
   "to": 1,
   "weight": 6,
   "message": "Examine edge E -> B (weight 6)"
  },
  {
   "type": "examine",
   "from": 4,
   "to": 2,
   "weight": 3,
   "message": "Examine edge E -> C (weight 3)"
  },
  {
   "type": "examine",
   "from": 4,
   "to": 5,
   "weight": 4,
   "message": "Examine edge E -> F (weight 4)"
  },
  {
   "type": "relax",
   "from": 4,
   "to": 5,
   "distance": 9,
   "message": "Update F: distance 9 via E"
  },
  {
   "type": "examine",
   "from": 4,
   "to": 6,
   "weight": 2,
   "message": "Examine edge E -> G (weight 2)"
  },
  {
   "type": "relax",
   "from": 4,
   "to": 6,
   "distance": 7,
   "message": "Update G: distance 7 via E"
  },
  {
   "type": "visit",
   "node": 3,
   "distance": 6,
   "message": "Visit D (distance 6)"
  },
  {
   "type": "examine",
   "from": 3,
   "to": 0,
   "weight": 7,
   "message": "Examine edge D -> A (weight 7)"
  },
  {
   "type": "examine",
   "from": 3,
   "to": 1,
   "weight": 2,
   "message": "Examine edge D -> B (weight 2)"
  },
  {
   "type": "examine",
   "from": 3,
   "to": 2,
   "weight": 5,
   "message": "Examine edge D -> C (weight 5)"
  },
  {
   "type": "examine",
   "from": 3,
   "to": 6,
   "weight": 4,
   "message": "Examine edge D -> G (weight 4)"
  },
  {
   "type": "examine",
   "from": 3,
   "to": 7,
   "weight": 9,
   "message": "Examine edge D -> H (weight 9)"
  },
  {
   "type": "relax",
   "from": 3,
   "to": 7,
   "distance": 15,
   "message": "Update H: distance 15 via D"
  },
  {
   "type": "stale",
   "node": 3,
   "distance": 7,
   "message": "Skip stale queue entry for D (7 > 6)"
  },
  {
   "type": "visit",
   "node": 6,
   "distance": 7,
   "message": "Visit G (distance 7)"
  },
  {
   "type": "examine",
   "from": 6,
   "to": 3,
   "weight": 4,
   "message": "Examine edge G -> D (weight 4)"
  },
  {
   "type": "examine",
   "from": 6,
   "to": 4,
   "weight": 2,
   "message": "Examine edge G -> E (weight 2)"
  },
  {
   "type": "examine",
   "from": 6,
   "to": 7,
   "weight": 3,
   "message": "Examine edge G -> H (weight 3)"
  },
  {
   "type": "relax",
   "from": 6,
   "to": 7,
   "distance": 10,
   "message": "Update H: distance 10 via G"
  },
  {
   "type": "visit",
   "node": 5,
   "distance": 9,
   "message": "Visit F (distance 9)"
  },
  {
   "type": "examine",
   "from": 5,
   "to": 4,
   "weight": 4,
   "message": "Examine edge F -> E (weight 4)"
  },
  {
   "type": "examine",
   "from": 5,
   "to": 7,
   "weight": 2,
   "message": "Examine edge F -> H (weight 2)"
  },
  {
   "type": "visit",
   "node": 7,
   "distance": 10,
   "message": "Visit H (distance 10)"
  },
  {
   "type": "done",
   "found": true,
   "message": "Reached H: shortest distance 10"
  }
 ],
 "stats": {
  "nodesVisited": 8,
  "edgesExamined": 23,
  "edgesRelaxed": 9,
  "queuePushes": 10,
  "queuePops": 9,
  "staleEntriesSkipped": 1,
  "timeMicroseconds": 3.2
 }
};
const MOCK_BFS = {
 "algorithm": "bfs",
 "source": 0,
 "target": 7,
 "found": true,
 "path": [
  0,
  3,
  7
 ],
 "distance": 16,
 "hops": 2,
 "steps": [
  {
   "type": "visit",
   "node": 0,
   "distance": 0,
   "message": "Visit A (0 edges from source)"
  },
  {
   "type": "examine",
   "from": 0,
   "to": 1,
   "weight": 4,
   "message": "Examine edge A -> B"
  },
  {
   "type": "relax",
   "from": 0,
   "to": 1,
   "distance": 1,
   "message": "Discover B: 1 edge via A"
  },
  {
   "type": "examine",
   "from": 0,
   "to": 2,
   "weight": 2,
   "message": "Examine edge A -> C"
  },
  {
   "type": "relax",
   "from": 0,
   "to": 2,
   "distance": 1,
   "message": "Discover C: 1 edge via A"
  },
  {
   "type": "examine",
   "from": 0,
   "to": 3,
   "weight": 7,
   "message": "Examine edge A -> D"
  },
  {
   "type": "relax",
   "from": 0,
   "to": 3,
   "distance": 1,
   "message": "Discover D: 1 edge via A"
  },
  {
   "type": "visit",
   "node": 1,
   "distance": 1,
   "message": "Visit B (1 edge from source)"
  },
  {
   "type": "examine",
   "from": 1,
   "to": 0,
   "weight": 4,
   "message": "Examine edge B -> A"
  },
  {
   "type": "examine",
   "from": 1,
   "to": 3,
   "weight": 2,
   "message": "Examine edge B -> D"
  },
  {
   "type": "examine",
   "from": 1,
   "to": 4,
   "weight": 6,
   "message": "Examine edge B -> E"
  },
  {
   "type": "relax",
   "from": 1,
   "to": 4,
   "distance": 2,
   "message": "Discover E: 2 edges via B"
  },
  {
   "type": "visit",
   "node": 2,
   "distance": 1,
   "message": "Visit C (1 edge from source)"
  },
  {
   "type": "examine",
   "from": 2,
   "to": 0,
   "weight": 2,
   "message": "Examine edge C -> A"
  },
  {
   "type": "examine",
   "from": 2,
   "to": 3,
   "weight": 5,
   "message": "Examine edge C -> D"
  },
  {
   "type": "examine",
   "from": 2,
   "to": 4,
   "weight": 3,
   "message": "Examine edge C -> E"
  },
  {
   "type": "visit",
   "node": 3,
   "distance": 1,
   "message": "Visit D (1 edge from source)"
  },
  {
   "type": "examine",
   "from": 3,
   "to": 0,
   "weight": 7,
   "message": "Examine edge D -> A"
  },
  {
   "type": "examine",
   "from": 3,
   "to": 1,
   "weight": 2,
   "message": "Examine edge D -> B"
  },
  {
   "type": "examine",
   "from": 3,
   "to": 2,
   "weight": 5,
   "message": "Examine edge D -> C"
  },
  {
   "type": "examine",
   "from": 3,
   "to": 6,
   "weight": 4,
   "message": "Examine edge D -> G"
  },
  {
   "type": "relax",
   "from": 3,
   "to": 6,
   "distance": 2,
   "message": "Discover G: 2 edges via D"
  },
  {
   "type": "examine",
   "from": 3,
   "to": 7,
   "weight": 9,
   "message": "Examine edge D -> H"
  },
  {
   "type": "relax",
   "from": 3,
   "to": 7,
   "distance": 2,
   "message": "Discover H: 2 edges via D"
  },
  {
   "type": "visit",
   "node": 4,
   "distance": 2,
   "message": "Visit E (2 edges from source)"
  },
  {
   "type": "examine",
   "from": 4,
   "to": 1,
   "weight": 6,
   "message": "Examine edge E -> B"
  },
  {
   "type": "examine",
   "from": 4,
   "to": 2,
   "weight": 3,
   "message": "Examine edge E -> C"
  },
  {
   "type": "examine",
   "from": 4,
   "to": 5,
   "weight": 4,
   "message": "Examine edge E -> F"
  },
  {
   "type": "relax",
   "from": 4,
   "to": 5,
   "distance": 3,
   "message": "Discover F: 3 edges via E"
  },
  {
   "type": "examine",
   "from": 4,
   "to": 6,
   "weight": 2,
   "message": "Examine edge E -> G"
  },
  {
   "type": "visit",
   "node": 6,
   "distance": 2,
   "message": "Visit G (2 edges from source)"
  },
  {
   "type": "examine",
   "from": 6,
   "to": 3,
   "weight": 4,
   "message": "Examine edge G -> D"
  },
  {
   "type": "examine",
   "from": 6,
   "to": 4,
   "weight": 2,
   "message": "Examine edge G -> E"
  },
  {
   "type": "examine",
   "from": 6,
   "to": 7,
   "weight": 3,
   "message": "Examine edge G -> H"
  },
  {
   "type": "visit",
   "node": 7,
   "distance": 2,
   "message": "Visit H (2 edges from source)"
  },
  {
   "type": "done",
   "found": true,
   "message": "Reached H in 2 edges (path cost 16)"
  }
 ],
 "stats": {
  "nodesVisited": 7,
  "edgesExamined": 21,
  "edgesRelaxed": 7,
  "queuePushes": 8,
  "queuePops": 7,
  "staleEntriesSkipped": 0,
  "timeMicroseconds": 1.4
 }
};

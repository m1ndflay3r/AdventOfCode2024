use std::collections::{HashMap, HashSet};
use std::fs;
use std::hash::{Hash, Hasher};

#[derive(Clone, Eq, PartialEq)]
struct DirectedEdge {
  from: (isize, isize),
  dir: (isize, isize),
}

impl Hash for DirectedEdge {
  fn hash<H: Hasher>(&self, state: &mut H) {
    self.from.hash(state);
    self.dir.hash(state);
  }
}

#[derive(Clone, Eq, PartialEq)]
struct SetWrapper {
  set: HashSet<(isize, isize)>,
}

impl Hash for SetWrapper {
  fn hash<H: Hasher>(&self, state: &mut H) {
    let mut elements: Vec<_> = self.set.iter().collect();
    elements.sort();
    for elem in elements {
      elem.hash(state);
    }
  }
}

fn edge(ps: &HashSet<(isize, isize)>) -> (HashSet<DirectedEdge>, HashSet<DirectedEdge>) {
  let directions = [(1, 0), (-1, 0), (0, 1), (0, -1)];
  let mut boundary_edges = HashSet::new();
  for &p in ps {
    for &d in &directions {
      let neighbor = (p.0 + d.0, p.1 + d.1);
      let edge = DirectedEdge { from: p, dir: d };
      if !ps.contains(&neighbor) {
        boundary_edges.insert(edge);
      }
    }
  }

  let interior_edges: HashSet<DirectedEdge> = boundary_edges
    .iter()
    .filter(|&edge| {
      let neighbor = (edge.from.0 + edge.dir.0, edge.from.1 + edge.dir.1);
      let reverse_edge = DirectedEdge {
        from: neighbor,
        dir: (-edge.dir.0, -edge.dir.1),
      };
      boundary_edges.contains(&reverse_edge)
    })
    .cloned()
    .collect();
  boundary_edges.retain(|e| !interior_edges.contains(e));
  (boundary_edges, interior_edges)
}

fn main() {
  // Read input and build the grid.
  let input = fs::read_to_string("input").expect("Cannot find puzzle input! Are you in the right directory?");
  let mut grid: HashMap<(isize, isize), char> = HashMap::new();
  for (i, line) in input.lines().enumerate() {
    for (j, c) in line.chars().enumerate() {
      grid.insert((i as isize, j as isize), c);
    }
  }
  // Initialize disjoint sets.
  let mut sets: HashMap<(isize, isize), HashSet<(isize, isize)>> = HashMap::new();
  for &p in grid.keys() {
    sets.insert(p, HashSet::from([p]));
  }
  // Merge sets for adjacent grid cells with the same value.
  for &p in grid.keys() {
    let neighbors = [
      (p.0 + 1, p.1), // down
      (p.0 - 1, p.1), // up
      (p.0, p.1 + 1), // right
      (p.0, p.1 - 1), // left
    ];
    for &n in &neighbors {
      if let Some(&grid_p) = grid.get(&p) {
        if let Some(&grid_n) = grid.get(&n) {
          if grid_p == grid_n {
            let mut union = sets.get(&p).unwrap().clone();
            union.extend(sets.get(&n).unwrap());
            for &x in &union {
              sets.insert(x, union.clone());
            }
          }
        }
      }
    }
  }
  // Reduce to unique sets using a HashSet for deduplication.
  let mut unique_sets: HashSet<SetWrapper> = HashSet::new();
  for set in sets.values() {
    unique_sets.insert(SetWrapper { set: set.clone() });
  }
  // Compute and print results.
  let mut boundary_result = 0;
  let mut interior_result = 0;
  for s in &unique_sets {
    let (boundary, interior) = edge(&s.set);
    boundary_result += s.set.len() * boundary.len();
    interior_result += s.set.len() * interior.len();
  }
  // Print the results.
  println!("{}", boundary_result); // First line of output
  println!("{}", interior_result); // Second line of output
}

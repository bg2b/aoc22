// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <cstdlib>
#include <cassert>

using namespace std;

// A pair with movement methods for clarity
struct knot_pos: pair<int, int> {
  knot_pos(int x = 0, int y = 0) : pair<int, int>(x, y) {}
  // Take a step in the given direction
  void step(char dir);
  // Follow another knot_pos
  void follow(knot_pos const &pos);
};

void knot_pos::step(char dir) {
  switch (dir) {
  case 'L':  --first; break;
  case 'R':  ++first; break;
  case 'U': ++second; break;
  case 'D': --second; break;
  default:
    assert(dir == 'L' || dir == 'R' || dir == 'U' || dir == 'D');
  }
}

void knot_pos::follow(knot_pos const &pos) {
  int dx = pos.first - first;
  int dy = pos.second - second;
  if (abs(dx) <= 1 && abs(dy) <= 1)
    // Close enough that no movement is needed
    return;
  auto ketchup = [](int d) { return d == 0 ? 0 : (d > 0 ? +1 : -1); };
  first += ketchup(dx);
  second += ketchup(dy);
  assert(abs(pos.first - first) <= 1 && abs(pos.second - second) <= 1);
}

void rope_sim(int num_knots) {
  assert(num_knots >= 2);
  // The rope
  vector<knot_pos> rope(num_knots);
  knot_pos &head = rope.front();
  knot_pos &tail = rope.back();
  // Where the tail has been
  set<knot_pos> tail_positions;
  tail_positions.emplace(tail);
  // Simulate
  char dir;
  int num_steps;
  while (cin >> dir >> num_steps) {
    for (int _ = 0; _ < num_steps; ++_) {
      head.step(dir);
      // Drag each knot after the previous one
      for (size_t i = 1; i < rope.size(); ++i)
        rope[i].follow(rope[i - 1]);
      tail_positions.emplace(tail);
    }
  }
  cout << tail_positions.size() << '\n';
}

void part1() { rope_sim(2); }

void part2() { rope_sim(10); }

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " partnum < input\n";
    exit(1);
  }
  if (*argv[1] == '1')
    part1();
  else
    part2();
  return 0;
}

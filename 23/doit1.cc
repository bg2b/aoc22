// -*- C++ -*-
// Optimization helpful for part 2
// g++ -std=c++17 -Wall -g -O -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return { c1.first + c2.first, c1.second + c2.second };
}

struct direction {
  // All these should be clear before trying to move in the proposed
  // direction
  vector<coord> check;
  // Offset to propose
  coord delta;
};

// Staying is always checked first
direction  stay{ { { -1, +1 }, { 0, +1 }, { +1, +1 },
                   { -1,  0 },            { +1,  0 },
                   { -1, -1 }, { 0, -1 }, { +1, -1 } },
                 { 0, 0 } };

// The rest of the directions cycle
direction north{ { { 0, +1 }, { -1, +1 }, { +1, +1 } }, { 0, +1 } };
direction south{ { { 0, -1 }, { -1, -1 }, { +1, -1 } }, { 0, -1 } };
direction west{ { { -1, 0 }, { -1, +1 }, { -1, -1 } }, { -1, 0 } };
direction east{ { { +1, 0 }, { +1, +1 }, { +1, -1 } }, { +1, 0 } };

vector<direction> directions { north, south, west, east };

struct elves {
  // Where the elves are and where they propose to move.  The proposal
  // part is irrelevant for the elf locations, but is used during a
  // round to avoid having to allocate a similar map telling where
  // each elf tries to move.
  map<coord, coord> locs;
  // First direction to try proposing
  unsigned first_dir{0};

  // Construct from stdin
  elves();

  // Is there an elf at pos?
  bool is_occupied(coord const &pos) const { return locs.count(pos); }
  // Try proposing some direction for the elf at pos, set next and
  // return true if it's a good proposal, else return false
  bool propose(coord const &pos, direction const &dir, coord &next) const;
  // Simulate one round, return true if anyone moved
  bool round();

  // How many empty spots are among the elves?
  size_t count_empty() const;
};

elves::elves() {
  string line;
  int y = 0;
  while (getline(cin, line)) {
    for (size_t x = 0; x < line.length(); ++x)
      if (line[x] == '#')
        // Second is a don't care at this point
        locs.emplace(coord({ x, y }), coord({ 0, 0 }));
    --y;
  }
}

bool elves::propose(coord const &pos, direction const &dir, coord &next) const {
  for (size_t i = 0; i < dir.check.size(); ++i)
    if (is_occupied(pos + dir.check[i]))
      return false;
  next = pos + dir.delta;
  return true;
}

bool elves::round() {
  map<coord, unsigned> wanted;
  for (auto &[pos, proposal] : locs) {
    proposal = pos;
    if (!propose(pos, stay, proposal))
      for (unsigned i = 0; i < 4; ++i)
        if (propose(pos, directions[(i + first_dir) % 4], proposal))
          break;
    ++wanted[proposal];
  }
  bool any_moved = false;
  map<coord, coord> next;
  for (auto const &[pos, proposal] : locs) {
    // Second is a don't care
    next.emplace(wanted[proposal] == 1 ? proposal : pos, coord({ 0, 0 }));
    any_moved = any_moved || pos != proposal;
  }
  assert(locs.size() == next.size());
  locs = next;
  ++first_dir;
  return any_moved;
}

size_t elves::count_empty() const {
  auto [min_x, min_y] = locs.begin()->first;
  int max_x = min_x;
  int max_y = min_y;
  for (auto const &p : locs) {
    auto [x, y] = p.first;
    min_x = min(min_x, x);
    max_x = max(max_x, x);
    min_y = min(min_y, y);
    max_y = max(max_y, y);
  }
  return (max_x - min_x + 1) * (max_y - min_y + 1) - locs.size();
}

void part1() {
  elves lvs;
  for (int _ = 0; _ < 10; ++_)
    lvs.round();
  cout << lvs.count_empty() << '\n';
}

void part2() {
  elves lvs;
  size_t steps;
  for (steps = 1; lvs.round(); ++steps)
    ;
  cout << steps << '\n';
}

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

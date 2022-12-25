// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <optional>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return { c1.first + c2.first, c1.second + c2.second };
}

int manhattan_dist(coord const &c1, coord const &c2) {
  return abs(c1.first - c2.first) + abs(c1.second - c2.second);
}

struct valley {
  vector<string> map;
  coord start;
  coord goal;

  valley();

  char at(unsigned r, unsigned c) const { return map[r][c]; }

  bool is_clear(coord const &pos, int t) const;
};

valley::valley() {
  string line;
  while (getline(cin, line)) {
    map.push_back(line);
    assert(line.length() == map.front().length());
  }
  start = { 0, map.front().find('.') };
  goal = { map.size() - 1, map.back().find('.') };
  assert(map.size() >= 3 && map.front().length() >= 3);
}

inline int unsigned_mod(int a, int modulus) {
  int result = a % modulus;
  if (result < 0)
    result += modulus;
  return result;
}

bool valley::is_clear(coord const &pos, int t) const {
  auto [r, c] = pos;
  int nr = map.size();
  int nc = map.front().length();
  if (r < 0 || r >= nr || c < 0 || c >= nc)
    // Moving off the map
    return false;
  if (at(r, c) == '#')
    // Wall
    return false;
  if (r == 0 || c == 0 || r == nr - 1 || c == nc - 1)
    // Must be either the start or the goal
    return true;
  // v blizzards move down (row + 1) each time step
  if (at(unsigned_mod(r - 1 - t, nr - 2) + 1, c) == 'v')
    return false;
  // ^ blizzards
  if (at((r - 1 + t) % (nr - 2) + 1, c) == '^')
    return false;
  // < blizzards
  if (at(r, (c - 1 + t) % (nc - 2) + 1) == '<')
    return false;
  // > blizzards
  if (at(r, unsigned_mod(c - 1 - t, nc - 2) + 1) == '>')
    return false;
  // Nothing here
  return true;
}

int search(valley const &v, coord const &from, coord const &to, int start_t) {
  using state = pair<coord, int>;
  // Prefer states closer to the goal and with smaller times.
  // Heuristically, about 2 time units gets us one step closer to the
  // goal.
  auto comp = [&](state const &s1, state const &s2) {
                int dist1 = manhattan_dist(s1.first, to) + s1.second / 2;
                int dist2 = manhattan_dist(s2.first, to) + s2.second / 2;
                // Want minimum at the front of the queue
                return dist1 > dist2;
              };
  priority_queue<state, vector<state>, decltype(comp)> queue(comp);
  set<state> visited;
  auto visit = [&](state const &s) {
                 if (visited.count(s))
                   return;
                 visited.insert(s);
                 queue.push(s);
               };
  visit({ from, 0 });
  optional<int> best;
  while (!queue.empty()) {
    auto [pos, t] = queue.top();
    queue.pop();
    if (pos == to) {
      // Reached goal, but must continue searching to ensure no other
      // path can get here faster
      best = min(best.value_or(t), t);
      continue;
    }
    if (best && manhattan_dist(pos, to) + t >= *best)
      // Can't possibly arrive earlier
      continue;
    // Generate successor states
    static vector<coord> deltas
      { { 0, 0 }, { +1, 0 }, { -1, 0 }, { 0, +1 }, { 0, -1 } };
    for (auto const &delta : deltas) {
      coord next = pos + delta;
      if (v.is_clear(next, t + 1 + start_t))
        visit({ next, t + 1 });
    }
  }
  assert(best);
  return *best;
}

void part1() {
  valley v;
  cout << search(v, v.start, v.goal, 0) << '\n';
}

void part2() {
  valley v;
  int trip1 = search(v, v.start, v.goal, 0);
  // Returning, may as well just start as early as possible.  If
  // necessary to wait it's always possible to just sit at v.goal.
  int trip2 = search(v, v.goal, v.start, trip1);
  // Ditto coming back again
  int trip3 = search(v, v.start, v.goal, trip1 + trip2);
  cout << trip1 + trip2 + trip3 << '\n';
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

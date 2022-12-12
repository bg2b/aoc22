// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <functional>
#include <utility>
#include <cassert>

using namespace std;

using pos = pair<int, int>;

struct height_map {
  vector<string> heights;
  pos start_end[2];

  height_map();

  int w() const { return heights[0].length(); }
  int h() const { return heights.size(); }

  pos start() const { return start_end[0]; }
  pos end() const { return start_end[1]; }

  bool valid(pos const &p) const {
    return p.first >= 0 && p.first < h() && p.second >= 0 && p.second < w();
  }

  // Height for valid positions, otherwise a height that's too big to
  // ever be reached by legal steps
  char at(pos const &p) const {
    return valid(p) ? heights[p.first][p.second] : 'z' + 2;
  }

  // All new positions that can be visited from p
  list<pos> steps_from(pos const &p, set<pos> const &visited) const;

  // Remap for part 2
  void flip();
};

height_map::height_map() {
  string line;
  while (getline(cin, line)) {
    char special[2] = { 'S', 'E' };
    for (int i = 0; i < 2; ++i) {
      auto pos = line.find(special[i]);
      if (pos != string::npos) {
        start_end[i] = make_pair(heights.size(), pos);
        line[pos] = i ? 'z' : 'a';
      }
    }
    heights.push_back(line);
    assert(int(line.length()) == w());
  }
  assert(at(start()) == 'a' && at(end()) == 'z');
}

list<pos> height_map::steps_from(pos const &p, set<pos> const &visited) const {
  list<pos> result;
  auto consider =
    [&](pos const &p1) {
      if (visited.find(p1) == visited.end() && at(p1) <= at(p) + 1)
        result.push_back(p1);
    };
  consider(make_pair(p.first + 1, p.second));
  consider(make_pair(p.first - 1, p.second));
  consider(make_pair(p.first, p.second + 1));
  consider(make_pair(p.first, p.second - 1));
  return result;
}

void height_map::flip() {
  // Searching from the starting z to some a, descending at most 1
  // each step is equivalent to flipping the heights and searching
  // from the starting a to some z while ascending at most 1 each
  // step.  Hence part 2 is just part 1 but with flipped heights (and
  // a slightly different ending condition).
  swap(start_end[0], start_end[1]);
  for (auto &row : heights)
    for (auto &h : row)
      h = 'a' + ('z' - h);
}

void bfs(height_map const &map, function<bool(pos const &)> done) {
  list<pair<pos, int>> frontier;
  set<pos> visited;
  auto visit = [&](pos const &p, int num_steps) {
                 frontier.emplace_back(p, num_steps);
                 visited.emplace(p);
               };
  visit(map.start(), 0);
  while (!done(frontier.front().first)) {
    auto [pos, num_steps] = frontier.front();
    frontier.pop_front();
    for (auto next : map.steps_from(pos, visited))
      visit(next, num_steps + 1);
    assert(!frontier.empty());
  }
  cout << frontier.front().second << '\n';
}

void part1() {
  height_map map;
  bfs(map, [&](pos const &p) { return p == map.end(); });
}

void part2() {
  height_map map;
  map.flip();
  bfs(map, [&](pos const &p) { return map.at(p) == 'z'; });
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

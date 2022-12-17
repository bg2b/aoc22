// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <cstdlib>
#include <cassert>

using namespace std;

struct valve {
  // Name of the valve
  string name;
  // How fast pressure is released when the valve is open
  int flow_rate;
  // Direct connections
  vector<string> tunnels;
  // Computed shortest path lengths to other valves
  map<string, int> shortest;

  valve(string const &s);
};

valve::valve(string const &s) {
  stringstream ss(s);
  string vlv, has, flow, rate, tnnls, lead, to, vlvs;
  ss >> vlv >> name >> has >> flow >> rate >> tnnls >> lead >> to >> vlvs;
  flow_rate = atoi(rate.c_str() + rate.find('=') + 1);
  string other;
  while (ss >> other) {
    if (other.back() == ',')
      other.pop_back();
    tunnels.push_back(other);
  }
}

struct path {
  // Current location of agent
  string loc{"AA"};
  // The time remaining
  int min_remaining;
  // The valves that have been opened
  set<string> opened;
  // Pressure relieved by opened valves
  int total{0};

  // Initially empty path
  path(int min_remaining_) : min_remaining(min_remaining_) {}

  // Extend the path and open some other valve
  path &extend(valve const &vlv, int time_to_open);

  // Are two paths disjoint?
  bool is_disjoint_with(path const &other) const;
};

path &path::extend(valve const &vlv, int time_to_open) {
  assert(!opened.count(vlv.name));
  loc = vlv.name;
  min_remaining -= time_to_open;
  assert(min_remaining > 0);
  opened.insert(vlv.name);
  total += min_remaining * vlv.flow_rate;
  return *this;
}

bool path::is_disjoint_with(path const &other) const {
  for (auto const &i : other.opened)
    if (opened.count(i))
      return false;
  return true;
}

struct volcano {
  // All valves by name
  map<string, valve> valves;
  // The names of valves that can relieve pressure
  set<string> working;

  // Construct from stdin
  volcano();

  // Compute shortest paths between valves
  void path_compression();

  // Find a valve
  valve const &at(string const &name) const { return valves.find(name)->second; }
  // Get shortest path between valves
  int shortest_path(string const &from, string const &to) const;

  // Extend a path in all possible ways, collect paths
  void paths(path const &p, list<path> &all_paths) const;
  // Get all paths of up to a certain length, sorted best to worst
  list<path> paths(int length) const;
};

volcano::volcano() {
  string line;
  while (getline(cin, line)) {
    valve vlv(line);
    if (vlv.flow_rate > 0)
      working.insert(vlv.name);
    valves.emplace(vlv.name, vlv);
  }
  path_compression();
}

void volcano::path_compression() {
  for (auto &i : valves) {
    string const &name = i.first;
    valve &vlv = i.second;
    set<string> visited;
    list<pair<string, int>> frontier;
    auto visit =
      [&](string const &next, int steps) {
        if (visited.count(next))
          return;
        visited.insert(next);
        frontier.emplace_back(next, steps);
        assert(vlv.shortest.find(next) == vlv.shortest.end());
        vlv.shortest.emplace(next, steps);
      };
    visit(name, 0);
    while (!frontier.empty()) {
      auto [loc, steps] = frontier.front();
      frontier.pop_front();
      auto const &vlv1 = at(loc);
      for (auto const &next : vlv1.tunnels)
        visit(next, steps + 1);
    }
  }
}

int volcano::shortest_path(string const &from, string const &to) const {
  auto const &vlv = at(from);
  return vlv.shortest.find(to)->second;
}

void volcano::paths(path const &p, list<path> &all_paths) const {
  all_paths.push_back(p);
  for (auto const &next : working) {
    if (p.opened.count(next))
      // Already opened
      continue;
    int time_to_open = shortest_path(p.loc, next) + 1;
    if (time_to_open >= p.min_remaining)
      // Unable to open the valve in time
      continue;
    paths(path(p).extend(at(next), time_to_open), all_paths);
  }
}

list<path> volcano::paths(int length) const {
  list<path> result;
  paths(path(length), result);
  result.sort([](path const &p1, path const &p2) {
                return p1.total > p2.total;
              });
  return result;
}

void part1() {
  volcano v;
  auto paths = v.paths(30);
  cout << paths.front().total << '\n';
}

void part2() {
  volcano v;
  auto paths = v.paths(26);
  // Disjoint paths are compatible; you and the elephant essentially
  // act independently.  So just find two disjoint paths with the best
  // total.
  int best = 0;
  for (auto i = paths.begin(); i != paths.end(); ++i)
    for (auto j = i; j != paths.end(); ++j)
      if (i->is_disjoint_with(*j)) {
        best = max(best, i->total + j->total);
        // No need to check more since the paths are in order
        break;
      }
  cout << best << '\n';
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

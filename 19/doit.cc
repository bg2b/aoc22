// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <array>
#include <cassert>

using namespace std;

// nr = num robot types = num resource types
enum resource { ore = 0, clay, obsidian, geode, nr };

struct blueprint {
  // ID number
  int id;
  // costs[robot][resource] = amount of resource used in producing robot
  int costs[nr][nr - 1];

  blueprint(string const &s);
};

blueprint::blueprint(string const &s) {
  for (int i = 0; i < nr; ++i)
    for (int j = 0; j < nr - 1; ++j)
      costs[i][j] = 0;
  stringstream ss(s);
  string blueprint, each, _ore, robot, _costs, _clay, _obsidian, _and, _geode;
  char colon;
  ss >> blueprint >> id >> colon;
  ss >> each >> _ore >> robot >> _costs >> costs[ore][ore] >> _ore;
  ss >> each >> _clay >> robot >> _costs >> costs[clay][ore] >> _ore;
  ss >> each >> _obsidian >> robot >> _costs >> costs[obsidian][ore] >> _ore
     >> _and >> costs[obsidian][clay] >> _clay;
  ss >> each >> _geode >> robot >> _costs >> costs[geode][ore] >> _ore
     >> _and >> costs[geode][obsidian] >> _obsidian;
  assert(ss);
}

vector<blueprint> read() {
  vector<blueprint> blueprints;
  string line;
  while (getline(cin, line))
    blueprints.emplace_back(line);
  return blueprints;
}

// Search state
struct state {
  // How long do we have left?
  int time_remaining;
  // How many of each type of robot do we have?
  array<int, nr> robots;
  // How many of each type of resource do we have?
  array<int, nr> resources;

  // Initial state with a given amount of time left
  state(int time_remaining_);

  // Advance one time step (robots produce resources)
  void advance();

  // Return a bound on the number of geodes we could wind up with
  int upper_bound(blueprint const &bp) const;
};

state::state(int time_remaining_) : time_remaining(time_remaining_) {
  robots[ore] = 1;
  for (int i = clay; i < nr; ++i)
    robots[i] = 0;
  for (int i = 0; i < nr; ++i)
    resources[i] = 0;
}

void state::advance() {
  --time_remaining;
  for (int i = 0; i < nr; ++i)
    resources[i] += robots[i];
}

int state::upper_bound(blueprint const &bp) const {
  // Maximum resources that could have been produced
  auto resource_bounds = resources;
  // Maximum new robots that could have been built
  array<int, nr> new_robots;
  // Initially no new robots
  for (int robot = ore; robot <= geode; ++robot)
    new_robots[robot] = 0;
  // For all the remaining time
  for (int _ = 0; _ < time_remaining; ++_) {
    // See about building robots
    array<int, nr> next_new_robots;
    for (int robot = ore; robot <= geode; ++robot) {
      auto const &needed = bp.costs[robot];
      // Could another robot of this type be built?
      int next_new = new_robots[robot] + 1;
      for (int resource = ore; resource <= obsidian; ++resource)
        if (next_new * needed[resource] > resource_bounds[resource]) {
          // No, there couldn't be enough resources to make another
          // robot by this time
          next_new = new_robots[robot];
          break;
        }
      next_new_robots[robot] = next_new;
    }
    // Robots produce resources
    for (int robot = ore; robot <= geode; ++robot)
      resource_bounds[robot] += robots[robot] + new_robots[robot];
    // New robots come online
    new_robots = next_new_robots;
  }
  return resource_bounds[geode];
}

void search(blueprint const &bp, state const &s, int &best) {
  if (s.time_remaining == 0) {
    best = max(best, s.resources[geode]);
    return;
  }
  if (s.upper_bound(bp) <= best)
    // Cut off
    return;
  state next(s);
  next.advance();
  // See about building robots.  Heuristically prefer to build the
  // later robots first to get the geode production chain cranked up.
  for (int robot = geode; robot >= ore; --robot) {
    bool can_produce = true;
    for (int resource = ore; resource <= obsidian; ++resource)
      if (bp.costs[robot][resource] > s.resources[resource])
        can_produce = false;
    if (can_produce) {
      state build_robot(next);
      for (int resource = ore; resource <= obsidian; ++resource)
        build_robot.resources[resource] -= bp.costs[robot][resource];
      ++build_robot.robots[robot];
      search(bp, build_robot, best);
    }
  }
  // Also might just have to wait for resources to accumulate
  search(bp, next, best);
}

void part1() {
  auto blueprints = read();
  int ans = 0;
  for (auto const &bp : blueprints) {
    int best = 0;
    search(bp, state(24), best);
    ans += bp.id * best;
  }
  cout << ans << '\n';
}

void part2() {
  auto blueprints = read();
  assert(blueprints.size() >= 3);
  int ans = 1;
  for (int i = 0; i < 3; ++i) {
    int best = 0;
    search(blueprints[i], state(32), best);
    ans *= best;
  }
  cout << ans << '\n';
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

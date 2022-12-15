// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cstdlib>
#include <cassert>

using namespace std;

// First element is <= second, single number is [i, i] as opposed to
// the usual C++ style of [i, i+1).  That's for symmetry in things
// like coverage().
using range = pair<int, int>;

// Call a vector of ranges "disjoint" if the ranges are sorted and not
// abutting or overlapping; when [i1, j1] appears before [i2, j2],
// then i2 > j1 + 1.  Note that i2 = j1 + 1 would imply that there
// should instead be one range [i1, j2].
//
// Add a range to a disjoint vector, giving a new disjoint vector.
vector<range> onion(vector<range> const &rs, range const &rnew) {
  if (rs.empty())
    return { rnew };
  vector<range> result;
  auto add = [&](range const &r) {
               if (result.empty() || result.back().second + 1 < r.first)
                 result.push_back(r);
               else
                 result.back().second = max(result.back().second, r.second);
             };
  size_t i = 0;
  while (i < rs.size() && rs[i].first < rnew.first) add(rs[i++]);
  add(rnew);
  while (i < rs.size()) add(rs[i++]);
  return result;
}

struct sensor {
  // Coordinates
  int x;
  int y;
  // Coordinates of detected beacon
  int beacon_x;
  int beacon_y;
  // Manhattan distance to beacon
  int dist_to_closest;

  sensor(string const &s);

  // Add our coverage range to a disjoint vector saying what's
  // excluded at y == row_y.  (The range also includes our own beacon
  // if that's on row_y.)
  void add(int row_y, vector<range> &covered) const;
};

sensor::sensor(string const &s) {
  auto convert = [&](size_t pos) { return atoi(s.c_str() + pos); };
  x = convert(s.find("x=") + 2);
  y = convert(s.find("y=") + 2);
  beacon_x = convert(s.rfind("x=") + 2);
  beacon_y = convert(s.rfind("y=") + 2);
  dist_to_closest = abs(beacon_x - x) + abs(beacon_y - y);
}

void sensor::add(int row_y, vector<range> &covered) const {
  int span = dist_to_closest - abs(row_y - y);
  if (span >= 0)
    covered = onion(covered, range(x - span, x + span));
}

vector<sensor> read() {
  vector<sensor> result;
  string line;
  while (getline(cin, line))
    result.emplace_back(line);
  return result;
}

vector<range> coverage(vector<sensor> const &sensors, int row_y) {
  vector<range> covered;
  for (auto const &sensor : sensors)
    sensor.add(row_y, covered);
  return covered;
}

int const max_coord = 4000000;

void part1() {
  auto sensors = read();
  int const target_y = max_coord / 2;
  auto covered = coverage(sensors, target_y);
  int ans = 0;
  for (auto const &r : covered)
    ans += r.second - r.first + 1;
  // Don't count detected beacons
  set<int> detected;
  for (auto const &sensor : sensors)
    if (sensor.beacon_y == target_y)
      detected.insert(sensor.beacon_x);
  cout << ans - detected.size() << '\n';
}

void part2() {
  auto sensors = read();
  long tuning_freq = -1;
  auto tuning = [&](int x, int y) { tuning_freq = long(max_coord) * x + y; };
  for (int y = 0; y <= max_coord; ++y) {
    auto covered = coverage(sensors, y);
    for (auto r : covered) {
      // Unfortunately both these cases are necessary in general;
      // consider if the non-excluded spot is at an edge.  Usually
      // there will be two ranges that bracket the spot though.
      if (r.second + 1 <= max_coord)
        tuning(r.second + 1, y);
      if (r.first - 1 >= 0)
        tuning(r.first - 1, y);
    }
  }
  assert(tuning_freq != -1);
  cout << tuning_freq << '\n';
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

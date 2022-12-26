// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit1 doit1.cc
// ./doit1 1 < input  # part 1
// ./doit1 2 < input  # part 2

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

inline int manhattan_dist(int x1, int y1, int x2, int y2) {
  return abs(x1 - x2) + abs(y1 - y2);
}

int const max_coord = 4000000;

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

  // How far to (x1, y1)?
  int range_to(int x1, int y1) const { return manhattan_dist(x, y, x1, y1); }
  // Is (x1, y1) in the coverage range?
  bool covers(int x1, int y1) const {
    return range_to(x1, y1) <= dist_to_closest;
  }

  // A vertical scan over all coordinates will work for part 2, but it
  // takes over a second even with -O and is not very elegant.  However
  // it's possible to scan many fewer y's as follows.  The common case
  // of finding the beacon (*) is something like this:
  //       /     /
  //      <     /
  //       \   / ^
  //        \ / / \ /
  //       \ X /   X
  //        X*X   / \ /
  //   \   / X \ /   V
  //    \ / / \ V
  //     V /
  //      /
  // X's are crossing lines, V or < or ^ is a corner of a sensor region,
  // etc.  Focus on what happens just above the *.  There are two sensor
  // regions that overlap, and the beacon's y coordinate is just below
  // where the overlap ends.  So the strategy is to look for pairs of
  // sensors sl and sr where sl's boundary covers part of sr's and vice
  // versa.  Then compute the y just below where their overlap ends and
  // scan that horizontal line as in part 1.
  //
  // This covers most cases, but may not work if the beacon is on the
  // valid region boundary.  A beacon on the top or bottom edge can be
  // found just by scanning those separately.  For a beacon on a
  // vertical edge, there's this situation:
  //   |  /
  //   \ /
  //   *X
  //   / \ /
  //   |  V
  //   |
  // So additionally look just above and below where a sensor's
  // coverage enters the valid region.

  // Find y for the common case described above; this sensor is the
  // one on the left (sl), sr is the sensor on the right
  void get_interesting_y(sensor const &sr, set<int> &ys) const;
  // Find y's for the border case
  void get_border_ys(set<int> &ys) const;
};

sensor::sensor(string const &s) {
  auto convert = [&](size_t pos) { return atoi(s.c_str() + pos); };
  x = convert(s.find("x=") + 2);
  y = convert(s.find("y=") + 2);
  beacon_x = convert(s.rfind("x=") + 2);
  beacon_y = convert(s.rfind("y=") + 2);
  dist_to_closest = manhattan_dist(x, y, beacon_x, beacon_y);
}

void sensor::add(int row_y, vector<range> &covered) const {
  int span = dist_to_closest - abs(row_y - y);
  if (span >= 0)
    covered = onion(covered, range(x - span, x + span));
}

void sensor::get_interesting_y(sensor const &sr, set<int> &ys) const {
  if (!sr.covers(x + dist_to_closest, y))
    return;
  int xr = sr.x - sr.dist_to_closest;
  int yr = sr.y;
  if (!covers(xr, yr))
    return;
  // Imagine a line diagonally down and right from (xr, yr); at what
  // y-coordinate does it exit coverage?  If xr is to the left of x,
  // then initial moves down and right don't change the range.
  if (xr < x) {
    yr += x - xr;
    xr = x;
  }
  // Similarly if yr is above y then initial moves down and right
  // don't change the range.
  if (yr < y) {
    xr += y - yr;
    yr = y;
  }
  // Now this is the situation (+ = this sensor, r = (xr, yr)):
  // +--------
  // |
  // |     r
  // |
  // At this point, further steps down and right will increase the
  // range, each step adding 2.
  int margin = dist_to_closest - range_to(xr, yr);
  // margin is the amount of distance before a change in range will
  // take the point outside.  How many steps is that?  When margin is
  // even, margin / 2 gets exactly to the boundary, then one more step
  // is outside.  When margin is odd, margin / 2 is just inside the
  // boundary (one away both horizontally and vertically) and one more
  // step crosses the boundary to just outside.
  int steps = margin / 2 + 1;
  ys.insert(yr + steps);
}

void sensor::get_border_ys(set<int> &ys) const {
  if (x - dist_to_closest <= 0) {
    // Left of coverage extends at least to boundary
    int extra = dist_to_closest - x;
    ys.insert(y - extra - 1);
    ys.insert(y + extra + 1);
  }
  if (x + dist_to_closest >= max_coord) {
    // Right of coverage extends at least to boundary
    int extra = x + dist_to_closest - max_coord;
    ys.insert(y - extra - 1);
    ys.insert(y + extra + 1);
  }
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
  set<int> interesting_ys;
  // Always scan top and bottom edges to avoid boundary conditions
  interesting_ys.insert(0);
  interesting_ys.insert(max_coord);
  for (auto const &s1 : sensors) {
    for (auto const &s2 : sensors)
      s1.get_interesting_y(s2, interesting_ys);
    s1.get_border_ys(interesting_ys);
  }
  long tuning_freq = -1;
  auto tuning = [&](int x, int y) { tuning_freq = long(max_coord) * x + y; };
  for (int y : interesting_ys) {
    if (y < 0 || y > max_coord)
      // Out of the valid space
      continue;
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

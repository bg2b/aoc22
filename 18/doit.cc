// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <array>
#include <vector>
#include <set>
#include <cassert>

using namespace std;

using coord = array<double, 3>;

set<coord> read() {
  set<coord> lava;
  char comma;
  double i0, i1, i2;
  while (cin >> i0 >> comma >> i1 >> comma >> i2)
    lava.insert({ i0, i1, i2 });
  return lava;
}

size_t count_faces(set<coord> const &cubes) {
  size_t num_faces = 0;
  auto add_face = [&](coord const &f) { if (!cubes.count(f)) ++num_faces; };
  auto add_cube = [&](coord const &c) {
                    for (double delta : { -1, +1 }) {
                      add_face({ c[0] + delta, c[1], c[2] });
                      add_face({ c[0], c[1] + delta, c[2] });
                      add_face({ c[0], c[1], c[2] + delta });
                    }
                  };
  for (auto const &c : cubes)
    add_cube(c);
  return num_faces;
}

void part1() {
  cout << count_faces(read()) << '\n';
}

void part2() {
  auto lava = read();
  // Bounds with padding to ensure that everything outside can be
  // reached from corner ll
  coord ll = *lava.begin();
  coord ur = ll;
  for (auto const &c : lava)
    for (int i = 0; i < 3; ++i) {
      ll[i] = min(ll[i], c[i] - 1);
      ur[i] = max(ur[i], c[i] + 1);
    }
  // Find the outside
  set<coord> outside;
  vector<coord> frontier;
  auto visit = [&](coord const &c) {
                 if (lava.count(c) || outside.count(c))
                   // In the lava or already explored
                   return;
                 for (int i = 0; i < 3; ++i)
                   if (c[i] < ll[i] || c[i] > ur[i])
                     // Going out of bounds
                     return;
                 outside.insert(c);
                 frontier.push_back(c);
               };
  // Everything outside has to be reachable from ll
  visit(ll);
  while (!frontier.empty()) {
    coord c = frontier.back();
    frontier.pop_back();
    // Only orthogonal steps allowed
    for (int i = 0; i < 3; ++i) {
      coord adj(c);
      adj[i] = c[i] + 1;
      visit(adj);
      adj[i] = c[i] - 1;
      visit(adj);
    }
  }
  // Don't count the outer surfaces
  int false_boundary = 0;
  false_boundary += 2 * (ur[0] - ll[0] + 1) * (ur[1] - ll[1] + 1);
  false_boundary += 2 * (ur[0] - ll[0] + 1) * (ur[2] - ll[2] + 1);
  false_boundary += 2 * (ur[1] - ll[1] + 1) * (ur[2] - ll[2] + 1);
  cout << count_faces(outside) - false_boundary << '\n';
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

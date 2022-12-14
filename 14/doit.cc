// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <optional>
#include <utility>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

struct cave {
  // Tiles that contain something, either rock or earlier sand
  map<coord, char> tiles;
  // Any sand reaching this level falls into the abyss
  int abyss{0};

  // Construct from stdin
  cave();

  // Add stuff
  void path(string const &s);
  void segment(coord c1, coord const &c2);
  void rock(coord const &c) { tiles[c] = 'X'; abyss = max(abyss, c.second); }
  void floor();

  // What's at a coordinate, ' ' for nothing
  char at(coord const &c) const {
    auto p = tiles.find(c);
    return p == tiles.end() ? ' ' : p->second;
  }

  // Add some sand, return 'A' for reaching the abyss, 'o' if blocked
  char reaches();
};

cave::cave() {
  string line;
  while (getline(cin, line))
    path(line);
  assert(at({ 500, 0 }) == ' ');
}

void cave::path(string const &s) {
  stringstream ss(s);
  coord c1, c2;
  char comma;
  string to;
  ss >> c1.first >> comma >> c1.second;
  while (ss >> to >> c2.first >> comma >> c2.second) {
    segment(c1, c2);
    c1 = c2;
  }
}

void cave::segment(coord c1, coord const &c2) {
  auto step = [](int d1, int d2) { return d1 == d2 ? 0 : (d1 < d2 ? +1 : -1); };
  int dx = step(c1.first, c2.first);
  int dy = step(c1.second, c2.second);
  rock(c1);
  while (c1 != c2) {
    c1.first += dx;
    c1.second += dy;
    rock(c1);
  }
}

void cave::floor() {
  // abyss has the maximum y seen so far
  int floor = abyss + 2;
  // Sand going diagonally can get floor units to the side
  segment({ 500 - floor - 1, floor }, { 500 + floor + 1, floor });
}

char cave::reaches() {
  coord c{ 500, 0 };
  if (at(c) != ' ')
    return at(c);
  while (c.second < abyss) {
    optional<coord> next;
    for (int dx : { 0, -1, +1 })
      if (!next && at({ c.first + dx, c.second + 1 }) == ' ')
        next = { c.first + dx, c.second + 1 };
    if (!next) {
      tiles[c] = 'o';
      return ' ';
    }
    c = *next;
  }
  return 'A';
}

void sand(cave &c, char goal) {
  int ans = 0;
  while (c.reaches() != goal)
    ++ans;
  cout << ans << '\n';
}

void part1() { cave c; sand(c, 'A'); }

void part2() { cave c; c.floor(); sand(c, 'o'); }

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

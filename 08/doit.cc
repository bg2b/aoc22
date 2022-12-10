// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cassert>

using namespace std;

struct trees {
  vector<string> tree_heights;

  trees();

  int w() const { return tree_heights[0].length(); }
  int h() const { return tree_heights.size(); }

  bool valid(int i, int j) const {
    return i >= 0 && i < h() && j >= 0 && j < w();
  }

  // Returns 0 for an invalid (i, j), otherwise the tree height (the
  // original character, like '0').  To scan, do:
  //   while (char t = at(i, j)) { ...stuff with t, update i and j... }
  char at(int i, int j) const {
    return valid(i, j) ? tree_heights[i][j] : 0;
  }

  // Scan from all four edges.  The scan function will be called four
  // times, with (i, j) starting from a point on an edge, and (di, dj)
  // indicating the direction to scan.  Do i += di; j += dj; to scan
  // over the row / column.
  void scan4(function<void(int i, int j, int di, int dj)> scan) const;
};

trees::trees() {
  string line;
  while (getline(cin, line)) {
    tree_heights.push_back(line);
    assert(int(line.length()) == w());
  }
}

void trees::scan4(function<void(int i, int j, int di, int dj)> scan) const {
  for (int i = 0; i < h(); ++i) {
    scan(i,       0, 0,  1);
    scan(i, w() - 1, 0, -1);
  }
  for (int j = 0; j < w(); ++j) {
    scan(      0, j,  1, 0);
    scan(h() - 1, j, -1, 0);
  }
}

void part1() {
  trees ts;
  vector<vector<bool>> visible(ts.h(), vector<bool>(ts.w(), false));
  ts.scan4([&](int i, int j, int di, int dj) {
             char max_height = '0' - 1;
             while (char t = ts.at(i, j)) {
               if (t > max_height)
                 visible[i][j] = true;
               max_height = max(max_height, t);
               i += di;
               j += dj;
             }
           });
  unsigned ans = 0;
  for (int i = 0; i < ts.h(); ++i)
    for (int j = 0; j < ts.w(); ++j)
      ans += visible[i][j];
  cout << ans << '\n';
}

void part2() {
  trees ts;
  vector<vector<unsigned>> scenic_score(ts.h(), vector<unsigned>(ts.w(), 1));
  ts.scan4([&](int i, int j, int di, int dj) {
             // num_seen[t] = number of trees that can be seen from a
             // tree of height t (0-indexed, i.e., subtract '0').
             // This starts at 0 for an edge.
             int const num_heights = 10;
             vector<unsigned> num_seen(num_heights, 0);
             while (char t = ts.at(i, j)) {
               t -= '0';
               assert(t < num_heights);
               scenic_score[i][j] *= num_seen[t];
               // Trees of <= t can't see farther than this tree
               for (char small = 0; small <= t; ++small)
                 num_seen[small] = 1;
               // Trees of > t can see this tree in addition to what
               // they could see before
               for (char big = t + 1; big < num_heights; ++big)
                 ++num_seen[big];
               i += di;
               j += dj;
             }
           });
  unsigned ans = 0;
  for (int i = 0; i < ts.h(); ++i)
    for (int j = 0; j < ts.w(); ++j)
      ans = max(ans, scenic_score[i][j]);
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

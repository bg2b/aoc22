// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

vector<int> Xecute() {
  vector<int> xs(1, 1);
  string instr;
  while (cin >> instr) {
    // noop just delays and addx takes a cycle before changing, so
    // either way for one cycle nothing will happen
    xs.push_back(xs.back());
    if (instr == "noop")
      continue;
    assert(instr == "addx");
    int delta;
    cin >> delta;
    assert(cin);
    xs.push_back(xs.back() + delta);
  }
  return xs;
}

void part1() {
  auto xs = Xecute();
  int ans = 0;
  for (int i = 20; i <= 220; i += 40)
    ans += i * xs[i - 1];
  cout << ans << '\n';
}

void part2() {
  auto xs = Xecute();
  unsigned i = 0;
  for (int r = 0; r < 6; ++r) {
    for (int c = 0; c < 40; ++c) {
      assert(i < xs.size());
      cout << ((c >= xs[i] - 1 && c <= xs[i] + 1) ? '@' : ' ');
      ++i;
    }
    cout << '\n';
  }
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

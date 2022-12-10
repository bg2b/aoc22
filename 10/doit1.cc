// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit1 doit1.cc
// ./doit1 1 < input  # part 1
// ./doit1 2 < input  # part 2

#include <iostream>
#include <functional>
#include <cassert>

using namespace std;

void Xecute(function<void(int t, int x)> tick) {
  int t = 1;
  int x = 1;
  string instr;
  while (cin >> instr) {
    if (instr == "noop")
      tick(t++, x);
    else {
      assert(instr == "addx");
      tick(t++, x);
      tick(t++, x);
      int delta;
      cin >> delta;
      assert(cin);
      x += delta;
    }
  }
}

void part1() {
  int ans = 0;
  Xecute([&](unsigned t, int x) { if ((t + 20) % 40 == 0) ans += t * x; });
  cout << ans << '\n';
}

void part2() {
  int c = 0;
  Xecute([&](unsigned t, int x) {
	   cout << ((c >= x - 1 && c <= x + 1) ? '@' : ' ');
	   if (++c == 40) { cout << '\n'; c = 0; }
	 });
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

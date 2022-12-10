// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit1 doit1.cc
// ./doit1 1 < input  # part 1
// ./doit1 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

void print_top(unsigned n) {
  vector<int> elves;
  int current_elf = 0;
  string line;
  while (getline(cin, line)) {
    if (line.empty()) {
      elves.push_back(current_elf);
      current_elf = 0;
    } else
      current_elf += stoi(line);
  }
  elves.push_back(current_elf);
  sort(elves.begin(), elves.end());
  assert(n <= elves.size());
  int ans = 0;
  while (n--) {
    ans += elves.back();
    elves.pop_back();
  }
  cout << ans << '\n';
}

void part1() { print_top(1); }

void part2() { print_top(3); }

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

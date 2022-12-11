// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

using namespace std;

vector<vector<int>> read() {
  vector<vector<int>> elves;
  bool next_elf = true;
  string line;
  while (getline(cin, line)) {
    if (line.empty()) {
      next_elf = true;
      continue;
    }
    if (next_elf) {
      elves.push_back(vector<int>());
      next_elf = false;
    }
    elves.back().push_back(stoi(line));
  }
  return elves;
}

void part1() {
  auto elves = read();
  int max_calories = 0;
  for (auto const &elf : elves)
    max_calories = max(max_calories, reduce(elf.begin(), elf.end()));
  cout << max_calories << '\n';
}

void part2() {
  auto elves = read();
  assert(elves.size() > 2);
  vector<int> calories;
  for (auto const &elf : elves)
    calories.push_back(reduce(elf.begin(), elf.end()));
  sort(calories.begin(), calories.end(), greater());
  int top3 = calories[0] + calories[1] + calories[2];
  cout << top3 << '\n';
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

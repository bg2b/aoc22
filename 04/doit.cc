// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <functional>
#include <cstdio>
#include <cassert>

using namespace std;

// s... = start, e... = end

void count_pairs(function<bool(int, int, int, int)> condition) {
  int total = 0;
  int s1, e1, s2, e2;
  while (scanf(" %d-%d,%d-%d ", &s1, &e1, &s2, &e2) == 4) {
    assert(s1 <= e1 && s2 <= e2);
    if (condition(s1, e1, s2, e2))
      ++total;
  }
  cout << total << '\n';
}

void part1() {
  count_pairs([](int s1, int e1, int s2, int e2) {
                return ((s1 <= s2 && e2 <= e1) ||
                        (s2 <= s1 && e1 <= e2));
              });
}

void part2() {
  count_pairs([](int s1, int e1, int s2, int e2) {
                return ((s1 <= e2 && e1 >= s2) ||
                        (s2 <= e1 && e2 >= s1));
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

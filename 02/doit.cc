// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <functional>
#include <cassert>

using namespace std;

int to_rps(char c) {
  if (c >= 'X' && c <= 'Z')
    return c - 'X';
  assert(c >= 'A' && c <= 'C');
  return c - 'A';
}

int choice_points(int rps) { return rps + 1; }

bool beats(int rps1, int rps2) { return rps1 == (rps2 + 1) % 3; }

int round_points(int me, int opponent) {
  if (beats(me, opponent))
    return 6;
  if (beats(opponent, me))
    return 0;
  assert(me == opponent);
  return 3;
}

void score(function<int(int, int)> strategy) {
  int my_score = 0;
  string line;
  while (getline(cin, line)) {
    assert(line.length() == 3 && line[1] == ' ');
    int opponent = to_rps(line[0]);
    int me = strategy(to_rps(line[2]), opponent);
    my_score += choice_points(me) + round_points(me, opponent);
  }
  cout << my_score << '\n';
}

void part1() {
  score([](int me, int opponent) { return me; });
}

void part2() {
  score([](int goal, int opponent) {
	  int me = (opponent + 2 + goal) % 3;
	  assert((goal == 0 && beats(opponent, me)) ||
		 (goal == 2 && beats(me, opponent)) ||
		 (goal == 1 && me == opponent));
	  return me;
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

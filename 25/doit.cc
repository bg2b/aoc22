// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>

using namespace std;

using num = long;

num to_decimal(string const &snafu) {
  num result = 0;
  num pow5 = 1;
  for (auto i = snafu.rbegin(); i != snafu.rend(); ++i) {
    int d = *i - '0';
    if (*i == '-')
      d = -1;
    else if (*i == '=')
      d = -2;
    result += d * pow5;
    pow5 *= 5;
  }
  return result;
}

string to_snafu(num n) {
  string result;
  do {
    int d = n % 5;
    int carry = d > 2 ? 1 : 0;
    char ch = d + '0';
    if (d == 3)
      ch = '=';
    else if (d == 4)
      ch = '-';
    result.push_back(ch);
    n = n / 5 + carry;
  } while (n != 0);
  reverse(result.begin(), result.end());
  return result;
}

void part1() {
  string line;
  num sum = 0;
  while (getline(cin, line))
    sum += to_decimal(line);
  cout << to_snafu(sum) << '\n';
}

void part2() {
  cout << "Start The Blender!\n";
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

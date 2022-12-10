// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cstdio>
#include <cassert>

using namespace std;

// The stacks are represented as a vector with one string per stack.
// Index 0 in one of the strings is the bottom of the corresponding
// stack.
vector<string> read_stacks() {
  // Snarf lines up to the first empty one
  vector<string> lines;
  string line;
  while (getline(cin, line) && !line.empty())
    lines.push_back(line);
  assert(!lines.empty());
  // Last line is just the labels for the stacks, which indicates how
  // many stacks there are
  auto stack_labels = lines.back();
  assert((stack_labels.length() + 1) % 4 == 0);
  size_t num_stacks = (stack_labels.length() + 1) / 4;
  lines.pop_back();
  vector<string> stacks(num_stacks);
  // Build stacks starting with the bottom layer
  while (!lines.empty()) {
    line = lines.back();
    assert(line.length() == stack_labels.length());
    lines.pop_back();
    for (size_t i = 0; i < num_stacks; ++i) {
      char item = line[4 * i + 1];
      if (item != ' ')
        stacks[i].push_back(item);
    }
  }
  return stacks;
}

// move() does the updating of stacks according to the type of crane
void crane_sim(function<void(unsigned count, string &from, string &to)> move) {
  auto stacks = read_stacks();
  unsigned count, from, to;
  while (scanf(" move %u from %u to %u ", &count, &from, &to) == 3) {
    assert(1 <= from && from <= stacks.size());
    assert(1 <= to && to <= stacks.size());
    --from;
    --to;
    move(count, stacks[from], stacks[to]);
  }
  for (auto const &stack : stacks) {
    assert(!stack.empty());
    cout << stack.back();
  }
  cout << '\n';
}

void part1() {
  crane_sim([](unsigned count, string &from, string &to) {
              for (unsigned i = 0; i < count; ++i) {
                assert(!from.empty());
                char item = from.back();
                from.pop_back();
                to.push_back(item);
              }
            });
}

void part2() {
  crane_sim([](unsigned count, string &from, string &to) {
              assert(from.length() >= count);
              string crates = from.substr(from.length() - count);
              from.resize(from.length() - count);
              to.append(crates);
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

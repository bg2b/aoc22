// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cassert>

using namespace std;

vector<string> read() {
  vector<string> packs;
  string line;
  while (getline(cin, line))
    packs.push_back(line);
  return packs;
}

int priority(char item) {
  if (item >= 'A' && item <= 'Z')
    return item - 'A' + 27;
  assert(item >= 'a' && item <= 'z');
  return item - 'a' + 1;
}

void part1() {
  auto packs = read();
  int total = 0;
  for (auto const &pack : packs) {
    auto half = pack.length() / 2;
    assert(pack.length() == 2 * half);
    auto compartment1 = pack.substr(0, half);
    auto compartment2 = pack.substr(half);
    auto common = compartment1.find_first_of(compartment2);
    assert(common != string::npos);
    total += priority(compartment1[common]);
  }
  cout << total << '\n';
}

#if 0
// Bit flicking approach, since the number of item types isn't large
void part2() {
  using bitmask = unsigned long;
  assert(sizeof(bitmask) >= 8);
  auto packs = read();
  assert(packs.size() % 3 == 0);
  // pps = pack priorities, pps[i] = bit mask for priorities of all
  // items in packs[i]
  vector<bitmask> pps;
  for (auto const &pack : packs) {
    bitmask p = 0;
    for (auto c : pack)
      p |= bitmask(1) << (priority(c) - 1);
    pps.push_back(p);
  }
  int total = 0;
  for (size_t i = 0; i < pps.size(); i += 3) {
    bitmask common = pps[i + 0] & pps[i + 1] & pps[i + 2];
    // Must have a single common item
    assert(common != 0 && ((common & (common - 1)) == 0));
    total += ffsl(common);
  }
  cout << total << '\n';
}
#endif

#if 0
// Merge-like string approach, not relying on number of item types
void part2() {
  auto packs = read();
  assert(packs.size() % 3 == 0);
  for (auto &pack : packs) {
    sort(pack.begin(), pack.end());
    pack.resize(unique(pack.begin(), pack.end()) - pack.begin());
  }
  int total = 0;
  for (size_t i = 0; i < packs.size(); i += 3) {
    auto const &pack1 = packs[i + 0];
    auto const &pack2 = packs[i + 1];
    auto const &pack3 = packs[i + 2];
    size_t p1 = 0, p2 = 0, p3 = 0;
    while (true) {
      assert(p1 < pack1.size() && p2 < pack2.size() && p3 < pack3.size());
      char c1 = pack1[p1];
      char c2 = pack2[p2];
      char c3 = pack3[p3];
      if (c1 < c2 || c1 < c3)
	++p1;
      else if (c2 < c1 || c2 < c3)
	++p2;
      else if (c3 < c1 || c3 < c2)
	++p3;
      else {
	assert(c1 == c2 && c1 == c3);
	total += priority(c1);
	break;
      }
    }
  }
  cout << total << '\n';
}
#endif

#if 0
// String set approach
void part2() {
  auto packs = read();
  assert(packs.size() % 3 == 0);
  for (auto &pack : packs) {
    sort(pack.begin(), pack.end());
    pack.resize(unique(pack.begin(), pack.end()) - pack.begin());
  }
  int total = 0;
  for (size_t i = 0; i < packs.size(); i += 3) {
    auto const &p1 = packs[i + 0];
    auto const &p2 = packs[i + 1];
    auto const &p3 = packs[i + 2];
    string in12;
    set_intersection(p1.begin(), p1.end(), p2.begin(), p2.end(), back_inserter(in12));
    string common;
    set_intersection(in12.begin(), in12.end(), p3.begin(), p3.end(), back_inserter(common));
    assert(common.size() == 1);
    total += priority(common[0]);
  }
  cout << total << '\n';
}
#endif

// Merge-like, but going from the other far end
void part2() {
  auto packs = read();
  assert(packs.size() % 3 == 0);
  for (auto &pack : packs) {
    sort(pack.begin(), pack.end());
  }
  int total = 0;
  for (size_t i = 0; i < packs.size(); i += 3) {
    auto &p1 = packs[i + 0];
    auto &p2 = packs[i + 1];
    auto &p3 = packs[i + 2];
    while (p1.back() != p2.back() || p1.back() != p3.back()) {
      while (p1.back() > p2.back() || p1.back() > p3.back()) p1.pop_back();
      while (p2.back() > p1.back() || p2.back() > p3.back()) p2.pop_back();
      while (p3.back() > p1.back() || p3.back() > p2.back()) p3.pop_back();
    }
    total += priority(p1.back());
  }
  cout << total << '\n';
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

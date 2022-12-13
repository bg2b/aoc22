// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cassert>

using namespace std;

struct packet {
  optional<int> v;
  vector<packet> items;

  // Single integer
  packet(int v_) : v(v_) {}
  // Empty list
  packet() = default;
  // Convert from input, advance s past whatever was converted
  packet(char const *&s);

  bool is_int() const { return v.has_value(); }
  int value() const { return v.value(); }

  // Wrap in a single-element list
  packet wrap() const;
};

packet::packet(char const *&s) {
  if (isdigit(*s)) {
    v = atoi(s);
    while (isdigit(*s))
      ++s;
  } else {
    assert(*s == '[');
    ++s;
    while (*s != ']') {
      items.emplace_back(s);
      if (*s == ',')
        ++s;
    }
    ++s;
  }
}

packet packet::wrap() const {
  packet result;
  result.items.emplace_back(*this);
  return result;
}

optional<packet> read_packet() {
  string line;
  while (getline(cin, line)) {
    if (line.empty())
      continue;
    char const *s = line.c_str();
    return packet(s);
  }
  return nullopt;
}

optional<bool> right_order(packet const &l, packet const &r) {
  if (l.is_int() && r.is_int()) {
    if (l.value() == r.value())
      return nullopt;
    return l.value() < r.value();
  }
  if (l.is_int())
    return right_order(l.wrap(), r);
  if (r.is_int())
    return right_order(l, r.wrap());
  for (size_t i = 0; i < l.items.size() && i < r.items.size(); ++i) {
    auto cmp = right_order(l.items[i], r.items[i]);
    if (cmp.has_value())
      return cmp;
  }
  if (l.items.size() == r.items.size())
    return nullopt;
  return l.items.size() < r.items.size();
}

bool operator<(packet const &p1, packet const &p2) {
  auto cmp = right_order(p1, p2);
  assert(cmp.has_value());
  return *cmp;
}

bool operator==(packet const &p1, packet const &p2) {
  return !right_order(p1, p2).has_value();
}

void part1() {
  int ans = 0;
  for (int index = 1; auto l = read_packet(); ++index)
    if (*l < *read_packet())
      ans += index;
  cout << ans << '\n';
}

void part2() {
  vector<packet> pkts;
  while (auto p = read_packet())
    pkts.push_back(*p);
  auto div2 = packet(2).wrap().wrap();
  auto div6 = packet(6).wrap().wrap();
  pkts.push_back(div2);
  pkts.push_back(div6);
  sort(pkts.begin(), pkts.end());
  auto index2 = find(pkts.begin(), pkts.end(), div2) - pkts.begin() + 1;
  auto index6 = find(pkts.begin(), pkts.end(), div6) - pkts.begin() + 1;
  cout << index2 * index6 << '\n';
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

// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <variant>
#include <cstdlib>
#include <cctype>
#include <cassert>

using namespace std;

struct packet: public variant<int, vector<packet>> {
  // Single integer
  packet(int v) : variant(v) {}
  // List
  packet(vector<packet> const &pkts = vector<packet>()) : variant(pkts) {}
};

packet read(char const *&s) {
  if (isdigit(*s)) {
    packet result(atoi(s));
    while (isdigit(*s))
      ++s;
    return result;
  }
  packet result;
  assert(*s == '[');
  ++s;
  while (*s != ']') {
    get<1>(result).emplace_back(read(s));
    if (*s == ',')
      ++s;
  }
  ++s;
  return result;
}

optional<packet> read_packet() {
  string line;
  while (getline(cin, line)) {
    if (line.empty())
      continue;
    char const *s = line.c_str();
    return read(s);
  }
  return nullopt;
}

packet wrap(packet const &p) { return packet(vector<packet>(1, p)); }

int compare(packet const &l, packet const &r) {
  if (l.index() != r.index())
    return compare(l.index() == 0 ? wrap(l) : l,
                   r.index() == 0 ? wrap(r) : r);
  if (l.index() == 0)
    return get<0>(l) - get<0>(r);
  for (size_t i = 0; i < get<1>(l).size() && i < get<1>(r).size(); ++i)
    if (int cmp = compare(get<1>(l)[i], get<1>(r)[i]))
      return cmp;
  return get<1>(l).size() - get<1>(r).size();
}

bool operator<(packet const &p1, packet const &p2) {
  return compare(p1, p2) < 0;
}

bool operator==(packet const &p1, packet const &p2) {
  return compare(p1, p2) == 0;
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
  auto div2 = wrap(wrap(packet(2)));
  auto div6 = wrap(wrap(packet(6)));
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

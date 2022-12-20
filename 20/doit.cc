// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>

using namespace std;

using num = signed long;

void shift(list<num> &l, list<num>::iterator &it) {
  long shifts = *it;
  long szm1 = l.size() - 1;
  if (shifts < 0)
    shifts = szm1 - (-shifts % szm1);  // How obvious...
  else
    shifts %= szm1;
  assert(shifts >= 0);
  if (shifts == 0)
    return;
  num n = *it;
  auto next = it;
  while (shifts-- >= 0)  // One extra iteration since insert() goes before
    if (++next == l.end())
      next = l.begin();
  l.erase(it);
  it = l.insert(next, n);
}

void solve(num decrypt, int num_iters) {
  list<num> mixed;
  num n;
  while (cin >> n)
    mixed.push_back(n * decrypt);
  vector<list<num>::iterator> mixing_order;
  for (auto i = mixed.begin(); i != mixed.end(); ++i)
    mixing_order.push_back(i);
  for (int _ = 0; _ < num_iters; ++_)
    for (auto &i : mixing_order)
      shift(mixed, i);
  vector<num> final(mixed.begin(), mixed.end());
  size_t zero = find(final.begin(), final.end(), 0) - final.begin();
  num ans = 0;
  for (size_t offset = zero + 1000; offset <= zero + 3000; offset += 1000)
    ans += final[offset % final.size()];
  cout << ans << '\n';
}

void part1() { solve(1, 1); }
void part2() { solve(811589153, 10); }

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

// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit1 doit1.cc
// ./doit1 1 < input  # part 1
// ./doit1 2 < input  # part 2

// Based on an observation from Footkick72.  Assuming that the
// expression for the root expressed in terms of a "human" variable
// does not have the human in the denominator, then you can consider
// everything as a complex number and do a normal evaluation.  Then
// for part 1, just add real and imag.  For part 2, you can solve for
// the necessary human contribution.  The actual input satisfies that
// condition (all the right operands of divisions evaluate to normal
// constants).  input2 is an example where this fails (but where the
// original doit.cc would work).

#include <iostream>
#include <string>
#include <complex>
#include <map>
#include <functional>
#include <optional>
#include <tuple>
#include <cctype>
#include <cassert>

using namespace std;

using num = complex<double>;
using operation = function<num(num, num)>;

map<string, operation> ops =
  {{ "+", [](num n1, num n2) { return n1 + n2; } },
   { "-", [](num n1, num n2) { return n1 - n2; } },
   { "*", [](num n1, num n2) { return n1 * n2; } },
   { "/", [](num n1, num n2) { return n1 / n2; } },
  };

struct monkey {
  string name;
  // Cache for an operation-computing monkey, else just the constant
  // being yelled
  optional<num> yelling;
  // Who the monkey listens to and what it computes
  optional<tuple<operation const &, string, string>> listening;

  monkey(string const &name_, num n) : name(name_), yelling(n) {}
  monkey(string const &name_, operation const &op,
         string const &first, string const &second) :
    name(name_), listening({ op, first, second}) {}

  // See what the monkey is yelling
  num yell();
};

map<string, monkey> monkeys;

void read() {
  string name;
  while (cin >> name) {
    assert(name.back() == ':');
    name.pop_back();
    assert(monkeys.find(name) == monkeys.end());
    string first;
    cin >> first;
    if (isdigit(first[0])) {
      num n = stol(first);
      if (name == "humn")
        n *= num(0.0, 1.0);
      monkeys.emplace(name, monkey(name, n));
      continue;
    }
    string op, second;
    cin >> op >> second;
    assert(cin);
    monkeys.emplace(name, monkey(name, ops[op], first, second));
  }
}

monkey &find(string const &name) {
  auto p = monkeys.find(name);
  assert(p != monkeys.end());
  return p->second;
}

num monkey::yell() {
  if (!yelling) {
    operation const &op = get<0>(*listening);
    num first = find(get<1>(*listening)).yell();
    num second = find(get<2>(*listening)).yell();
    *yelling = op(first, second);
  }
  return *yelling;
}

void part1() {
  read();
  num n = find("root").yell();
  cout << long(round(real(n) + imag(n))) << '\n';
}

void part2() {
  read();
  auto &root = find("root");
  auto left = find(get<1>(*root.listening)).yell();
  auto right = find(get<2>(*root.listening)).yell();
  if (imag(left) == 0.0)
    swap(left, right);
  assert(imag(left) != 0.0 && imag(right) == 0.0);
  // What would a human yelling 1 contribute?
  double unit_human = imag(left) / imag(find("humn").yell());
  cout << long(round((real(right) - real(left)) / unit_human)) << '\n';
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

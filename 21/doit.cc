// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <optional>
#include <tuple>
#include <cctype>
#include <cassert>

using namespace std;

using num = long;

struct operation {
  // Regular evaluation function
  function<num(num, num)> eval;
  // How to solve first for wanted == first op given
  function<num(num, num)> solve1;
  // How to solve second for wanted == given op second
  function<num(num, num)> solve2;

  operation() = default;
  operation(function<num(num, num)> eval_, function<num(num, num)> solve1_,
            function<num(num, num)> solve2_) :
    eval(eval_), solve1(solve1_), solve2(solve2_) {}
};

map<string, operation> ops =
  {{ "+", operation([](num n1, num n2) { return n1 + n2; },
                    [](num res, num n2) { return res - n2; },
                    [](num res, num n1) { return res - n1; }) },
   { "-", operation([](num n1, num n2) { return n1 - n2; },
                    [](num res, num n2) { return res + n2; },
                    [](num res, num n1) { return n1 - res; }) },
   { "*", operation([](num n1, num n2) { return n1 * n2; },
                    [](num res, num n2) { return res / n2; },
                    [](num res, num n1) { return res / n1; }) },
   { "/", operation([](num n1, num n2) { return n1 / n2; },
                    [](num res, num n2) { return res * n2; },
                    [](num res, num n1) { return n1 / res; }) },
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

  // Does this monkey's yell depend on the human?
  bool depends_on_human() const;
  // What should the human yell to make this monkey yell wanted?
  num solve(num wanted);
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
      monkeys.emplace(name, monkey(name, stol(first)));
      continue;
    }
    string op, second;
    cin >> op >> second;
    assert(cin);
    assert(ops.find(op) != ops.end());
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
    *yelling = op.eval(first, second);
  }
  return *yelling;
}

bool monkey::depends_on_human() const {
  if (name == "humn")
    return true;
  if (!listening)
    return false;
  return (find(get<1>(*listening)).depends_on_human() ||
          find(get<2>(*listening)).depends_on_human());
}

num monkey::solve(num wanted) {
  if (name == "humn")
    return wanted;
  assert(listening);
  auto &first = find(get<1>(*listening));
  auto &second = find(get<2>(*listening));
  if (name == "root") {
    if (first.depends_on_human())
      return first.solve(second.yell());
    else
      return second.solve(first.yell());
  }
  auto const &op = get<0>(*listening);
  if (first.depends_on_human())
    return first.solve(op.solve1(wanted, second.yell()));
  else
    return second.solve(op.solve2(wanted, first.yell()));
}

void part1() {
  read();
  cout << find("root").yell() << '\n';
}

void part2() {
  read();
  cout << find("root").solve(1) << '\n';
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

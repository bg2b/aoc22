// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <functional>
#include <numeric>
#include <algorithm>
#include <cctype>
#include <cassert>

using namespace std;

// Just in case the organizers have been up to monkey business and
// written something that overflows 32-bit values (turned out to be
// prescient)
using num = unsigned long;

// Get the next input token
string token() {
  string token;
  cin >> token;
  assert(cin);
  return token;
}

// Consume some expected word
void expect(string const &word) {
  auto t = token();
  assert(t == word);
}

// Get some numbers, list continues as long as there are comma
// separators
list<num> numbers() {
  list<num> ns;
  bool num_expected = true;
  do {
    auto t = token();
    ns.emplace_back(stoul(t));
    num_expected = t.back() == ',';
  } while (num_expected);
  return ns;
}

// Get exactly one number
num number() {
  auto ns = numbers();
  assert(ns.size() == 1);
  return ns.front();
}

// One of the little devils
struct monkey {
  // What the monkey currently has
  list<num> holding;
  // How it updates the worry level
  function<num(num)> update;
  // What the monkey checks for divibility by
  unsigned divisor;
  // Which monkeys it throws to
  unsigned recipient[2];
  // How many items the monkey has handled
  unsigned num_handled{0};

  // Read and construct from stdin
  monkey();

  // Catch an item thrown by another monkey
  void receive(num worry_level) { holding.push_back(worry_level); }

  // Do one turn, throwing items to other monkeys
  void turn(vector<monkey> &monkeys, unsigned relief);

  // LCM of divisors of all monkeys, for reducing
  static num lcm_divisors;
};

num monkey::lcm_divisors = 1;

// For conciseness...
#define e(word) expect(word)

monkey::monkey() {
  e("Monkey"); (void)number();          // colon was eaten by number()
  e("Starting"); e("items:"); holding = numbers();
  e("Operation:"); e("new"); e("="); e("old");
  auto op = token();
  if (op == "+") {
    num v = number();
    update = [=](num n) { return n + v; };
  } else {
    assert(op == "*");
    // Could be multiplying by a number or squaring
    auto t = token();
    if (t == "old")
      update = [](num n) { return n * n; };
    else {
      num v = stoul(t);
      update = [=](num n) { return n * v; };
    }
  }
  e("Test:"); e("divisible"); e("by");
  divisor = number();
  lcm_divisors = lcm(lcm_divisors, divisor);
  e("If"); e("true:"); e("throw"); e("to"); e("monkey");
  recipient[0] = number();
  e("If"); e("false:"); e("throw"); e("to"); e("monkey");
  recipient[1] = number();
}

#undef e

void monkey::turn(vector<monkey> &monkeys, unsigned relief) {
  while (!holding.empty()) {
    // Monkey takes an item
    num worry_level = holding.front();
    holding.pop_front();
    ++num_handled;
    // Worry level update during inspection
    worry_level = update(worry_level);
    // Relief that item was not damaged
    worry_level /= relief;
    // Reduce, preserving divisibility for all monkeys
    worry_level %= lcm_divisors;
    // Throw to the next monkey
    unsigned throw_to = recipient[worry_level % divisor != 0];
    assert(throw_to < monkeys.size());
    monkeys[throw_to].receive(worry_level);
  }
}

// Read all monkeys
vector<monkey> read() {
  auto another_monkey = []() {
                          while (cin && isspace(cin.peek()))
                            cin.ignore(1);
                          return !cin.eof();
                        };
  vector<monkey> monkeys;
  while (another_monkey())
    monkeys.emplace_back();
  return monkeys;
}

void monkey_business(unsigned num_rounds, unsigned relief) {
  auto monkeys = read();
  for (unsigned _ = 0; _ < num_rounds; ++_)
    for (auto &m : monkeys)
      m.turn(monkeys, relief);
  vector<size_t> activity;
  for (auto const &m : monkeys)
    activity.push_back(m.num_handled);
  sort(activity.begin(), activity.end(), greater());
  assert(activity.size() >= 2);
  cout << activity[0] * activity[1] << '\n';
}

void part1() { monkey_business(20, 3); }

void part2() { monkey_business(10000, 1); }

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

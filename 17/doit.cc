// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cassert>

using namespace std;

string const nothing = ".......";

// Note: these are reversed top-to-bottom, i.e., rocks[_][0] is the
// bottom of the rock
vector<vector<string>> const rocks =
{
 { "@@@@"
 },
 { ".@.",
   "@@@",
   ".@."
 },
 { "@@@",  // see note above
   "..@",
   "..@"
 },
 { "@",
   "@",
   "@",
   "@"
 },
 { "@@",
   "@@"
 }
};

struct chamber {
  // What's in the chamber
  vector<string> contents;
  // Air jet directions
  string jets;
  // The next air direction is jets[next_jet]
  size_t next_jet{0};
  // The next thing to fall is rocks[next_rock]
  size_t next_rock{0};
  // Total rocks that have fallen
  size_t num_rocks{0};
  // When the next_rock is 0 (the horizontal 4-rock) and the top of
  // the filled space is effectively a floor (does not contain ....),
  // then the upcoming drops are just a function of next_jet.  This
  // table maps values of next_jet to the number of rocks and the
  // height in such configurations.
  map<size_t, pair<size_t, size_t>> possible_repeats;

  chamber();

  vector<string> const &get_rock();
  char get_jet();

  size_t height() const { return contents.size(); }

  // Trim off empty space at the top
  void trim();
  // Does the given rock fit into the chamber at the specified lower
  // left coordinates?
  bool fits(vector<string> const &rock, int x, int y) const;
  // Add the rock after it comes to rest
  void finish(vector<string> const &rock, int x, int y);
  // Drop a rock into the chamber.  Return { 0, 0 } if no repeat is
  // detected, else return { rocks in repeat, repeat height }
  pair<size_t, size_t> drop_rock();

  // Are we about to drop rock @@@@ into the chamber where it's
  // guaranteed not to nestle into a previous level regardless of the
  // air jets?
  bool like_a_floor() const;
  // If the next rock drop would start a repeating cycle, return the
  // number of rocks and the added height in the cycle.
  pair<size_t, size_t> check_for_repeats();
};

chamber::chamber() {
  getline(cin, jets);
}

vector<string> const &chamber::get_rock() {
  auto const &rock = rocks[next_rock];
  next_rock = (next_rock + 1) % rocks.size();
  return rock;
}

char chamber::get_jet() {
  char jet = jets[next_jet];
  next_jet = (next_jet + 1) % jets.size();
  return jet;
}

void chamber::trim() {
  while (!contents.empty() && contents.back() == nothing)
    contents.pop_back();
}

bool chamber::fits(vector<string> const &rock, int x, int y) const {
  if (x < 0 || x + rock[0].length() > nothing.length() || y < 0)
    return false;
  for (unsigned dy = 0; dy < rock.size(); ++dy)
    for (unsigned dx = 0; dx < rock[dy].length(); ++dx)
      if (rock[dy][dx] != '.' && contents[y + dy][x + dx] != '.')
        return false;
  return true;
}

void chamber::finish(vector<string> const &rock, int x, int y) {
  for (unsigned dy = 0; dy < rock.size(); ++dy)
    for (unsigned dx = 0; dx < rock[dy].length(); ++dx)
      // Danger Will Robinson!
      // NOT contents[y + dy][x + dx] = rock[dy][dx]
      // Ask me how I know...
      if (rock[dy][dx] != '.') {
        assert(contents[y + dy][x + dx] == '.');
        contents[y + dy][x + dx] = rock[dy][dx];
      }
  ++num_rocks;
}

pair<size_t, size_t> chamber::drop_rock() {
  for (int _ = 0; _ < 3; ++_)
    contents.push_back(nothing);
  auto const &rock = get_rock();
  int rock_left = 2;
  int rock_bot = contents.size();
  for (size_t _ = 0; _ < rock.size(); ++_)
    contents.push_back(nothing);
  while (true) {
    int dir = get_jet() == '<' ? -1 : +1;
    if (fits(rock, rock_left + dir, rock_bot))
      rock_left += dir;
    if (fits(rock, rock_left, rock_bot - 1))
      --rock_bot;
    else
      break;
  }
  finish(rock, rock_left, rock_bot);
  trim();
  return check_for_repeats();
}

bool chamber::like_a_floor() const {
  assert(!contents.empty());
  return next_rock == 0 && contents.back().find("....") == string::npos;
}

pair<size_t, size_t> chamber::check_for_repeats() {
  if (!like_a_floor())
    return { 0, 0 };
  auto pos = possible_repeats.find(next_jet);
  if (pos == possible_repeats.end()) {
    possible_repeats.emplace(next_jet, make_pair(num_rocks, height()));
    return { 0, 0 };
  }
  // Repeat detected
  auto [prev_num_rocks, prev_height] = pos->second;
  return { num_rocks - prev_num_rocks, height() - prev_height };
}

void solve(size_t remaining) {
  chamber c;
  pair<size_t, size_t> repeat;
  do {
    repeat = c.drop_rock();
    --remaining;
  } while (remaining > 0 && repeat.first == 0);
  auto [repeat_rocks, repeat_height] = repeat;
  if (repeat_rocks > 0) {
    size_t extra_repeats = remaining / repeat_rocks;
    remaining -= extra_repeats * repeat_rocks;
    repeat_height *= extra_repeats;
  }
  while (remaining > 0) {
    c.drop_rock();
    --remaining;
  }
  cout << c.height() + repeat_height << '\n';
}

void part1() { solve(2022); }
void part2() { solve(1000000000000); }

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

// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <array>
#include <algorithm>
#include <cassert>

using namespace std;

#if 0
// Naive
void find_sync(size_t n) {
  string line;
  getline(cin, line);
  auto sync = [&](size_t pos) {
		for (size_t i = 0; i < n; ++i)
		  for (size_t j = i + 1; j < n; ++j)
		    if (line[pos - i] == line[pos - j])
		      return false;
		return true;
	      };
  size_t pos;
  for (pos = n - 1; pos < line.length() && !sync(pos); ++pos)
    ;
  assert(pos < line.length());
  cout << pos + 1 << '\n';
}
#endif

#if 0
// Boyer-Moore-like
void find_sync(size_t n) {
  string line;
  getline(cin, line);
  // Suppose I'm scanning as above and find pos-i and pos-j are dups.
  // To skip past that failure requires that I not include pos-j,
  // i.e., the next possible sync should span n characters starting at
  // pos-j+1.  That span ends at pos-j+1+(n-1) == pos+(n-j).  Hence
  // for the next check I'll skip by n-j.
  //
  // To maximize efficiency, I want n-j to be large.  The largest
  // possible skip is n-1, and the smallest is 1.  I'll make the outer
  // loop over the possible skips from largest to smallest. Then since
  // skip == n-j, j == n-skip.  And i runs from 0 and is less than j.
  auto sync = [&](size_t &pos) {
		for (size_t skip = n - 1; skip > 0; --skip) {
		  size_t j = n - skip;
		  for (size_t i = 0; i < j; ++i)
		    if (line[pos - i] == line[pos - j]) {
		      pos += skip;
		      return false;
		    }
		}
		return true;
	      };
  size_t pos;
  for (pos = n - 1; pos < line.length() && !sync(pos); )
    ;
  assert(pos < line.length());
  cout << pos + 1 << '\n';
}
#endif

// Set-based
void find_sync(size_t n) {
  string line;
  getline(cin, line);
  array<unsigned, 256> occurrences;
  fill(occurrences.begin(), occurrences.end(), 0);
  unsigned num_unique = 0;
  size_t i;
  for (i = 0; i < line.length() && num_unique < n; ++i) {
    if (i >= n && --occurrences[line[i - n]] == 0)
      --num_unique;
    if (occurrences[line[i]]++ == 0)
      ++num_unique;
  }
  // The ++i after the last character in the sync happens before the
  // loop breaks with num_unique == n
  cout << i << '\n';
}

void part1() { find_sync(4); }

void part2() { find_sync(14); }

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

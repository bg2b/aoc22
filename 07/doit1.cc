// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit1 doit1.cc
// ./doit1 1 < input  # part 1
// ./doit1 2 < input  # part 2

#include <iostream>
#include <string>
#include <functional>
#include <cassert>

using namespace std;

// This assumes that the commands are a simple encoding of the
// directory tree with no funny business
vector<size_t> read() {
  // Sizes of completed directories
  vector<size_t> dir_sizes;
  // Sizes of directories that are being traversed
  vector<size_t> sizes;
  // Pop a directory from the traversal stack and save the size
  auto pop_dir = [&]() {
                   size_t dir_size = sizes.back();
                   sizes.pop_back();
                   // Save size of this directory
                   dir_sizes.push_back(dir_size);
                   // This directory's size is also part of its parent
                   if (!sizes.empty())
                     sizes.back() += dir_size;
                 };
  string line;
  while (getline(cin, line)) {
    if (line == "$ cd ..") {
      // Pop, size of current directory is complete
      assert(!sizes.empty());
      pop_dir();
    } else if (line.find("$ cd ") == 0) {
      // Starting a new directory
      sizes.push_back(0);
    } else if (line == "$ ls") {
      // Ignore list, entries will trigger the traversal
      ;
    } else if (line.find("dir ") == 0) {
      // Ignore directory name, later cd will deal with it
      ;
    } else {
      // Regular file, add to tally of directory
      size_t size = stoul(line);
      assert(size > 0 && !sizes.empty());
      sizes.back() += size;
    }
  }
  // Pop and accumulate sizes back to root
  while (!sizes.empty())
    pop_dir();
  return dir_sizes;
}

void part1() {
  auto dir_sizes = read();
  size_t ans = 0;
  for (auto dir_size : dir_sizes)
    if (dir_size <= 100000)
      ans += dir_size;
  cout << ans << '\n';
}

void part2() {
  auto dir_sizes = read();
  assert(!dir_sizes.empty());
  size_t const total_disk = 70000000;
  size_t const required = 30000000;
  // Root is the last item
  size_t used = dir_sizes.back();
  size_t free = total_disk - used;
  assert(free < required);
  size_t ans = used;                    // Can always rm -rf /
  for (auto dir_size : dir_sizes)
    if (free + dir_size >= required && dir_size < ans)
      ans = dir_size;
  cout << ans << '\n';
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

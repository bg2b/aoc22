// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <cassert>

using namespace std;

struct inode {
  // Size for files, 0 for directories
  size_t size;
  // Entries in a directory, possibly files, possibly subdirectories
  map<string, inode> contents;
  // Parent inode, null for root
  inode *parent;

  inode(inode *parent_ = nullptr, size_t size_ = 0);

  bool is_directory() const { return size == 0; }

  // Create a new directory entry with the specified size
  void make_entry(string const &new_name, size_t new_size = 0);

  // For debugging
  void print(string const &name) const;

  // Compute total size of subtrees.  For directories, call a function
  // which receives the directory's total size as argument.
  size_t total_size(function<void(size_t dir_size)> process_dir) const;
};

inode::inode(inode *parent_, size_t size_) : size(size_), parent(parent_) {}

void inode::make_entry(string const &new_name, size_t new_size) {
  assert(contents.find(new_name) == contents.end());
  contents[new_name] = inode(this, new_size);
}

void inode::print(string const &name) const {
  for (auto i = this; i->parent; i = i->parent)
    cout << "  ";
  if (is_directory()) {
    cout << name << " (dir)\n";
    for (auto const &i : contents)
      i.second.print(i.first);
  } else {
    assert(contents.empty());
    cout << name << " (file, size=" << size << ")\n";
  }
}

size_t inode::total_size(function<void(size_t dir_size)> process_dir) const {
  if (!is_directory())
    return size;
  size_t dir_size = 0;
  for (auto const &i : contents)
    dir_size += i.second.total_size(process_dir);
  process_dir(dir_size);
  return dir_size;
}

inode root;

// Look for a prefix in the command line
//    Remove it and return true if found
//    Leave the line alone and return false otherwise
bool scan(string &line, string const &prefix) {
  if (line.substr(0, prefix.length()) != prefix)
    return false;
  line = line.substr(prefix.length());
  return true;
}

void read() {
  inode *cwd = &root;
  string line;
  while (getline(cin, line)) {
    if (scan(line, "$ cd /")) {
      // Back to root
      cwd = &root;
    } else if (scan(line, "$ cd ..")) {
      // Up
      assert(cwd->parent);
      cwd = cwd->parent;
    } else if (scan(line, "$ cd ")) {
      // line now has the child subdirectory name
      auto child = cwd->contents.find(line);
      assert(child != cwd->contents.end());
      cwd = &child->second;
    } else if (scan(line, "$ ls")) {
      // List does nothing itself; the entries will populate the file
      // system
      ;
    } else if (scan(line, "dir ")) {
      // line now has the name of a directory
      cwd->make_entry(line);
    } else {
      // Regular file
      size_t pos;
      size_t size = stoul(line, &pos);
      assert(size > 0 && pos + 1 < line.length());
      cwd->make_entry(line.substr(pos + 1), size);
    }
  }
}

void part1() {
  read();
  size_t ans = 0;
  root.total_size([&](size_t dir_size) {
		    if (dir_size <= 100000) ans += dir_size;
		  });
  cout << ans << '\n';
}

void part2() {
  read();
  size_t const total_disk = 70000000;
  size_t const required = 30000000;
  size_t used = root.total_size([](size_t) {});
  size_t free = total_disk - used;
  assert(free < required);
  size_t ans = used;			// Can always rm -rf /
  root.total_size([&](size_t dir_size) {
		    if (free + dir_size >= required && dir_size < ans)
		      ans = dir_size;
		  });
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

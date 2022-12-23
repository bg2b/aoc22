// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <cstdlib>
#include <cctype>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return { c1.first + c2.first, c1.second + c2.second };
}

coord operator-(coord const &c1, coord const &c2) {
  return { c1.first - c2.first, c1.second - c2.second };
}

coord operator*(double sc, coord const &c) {
  return { sc * c.first, sc * c.second };
}

// The main representation of the board is just as a vector of
// strings, basically directly as read.  Normal movement just
// references that.  But for wrapping on the cube, there's additional
// information oriented around faces.  There will be six faces that
// cover everything nonempty in the main representation.
//
// Specifically, faces are square of size side * side at various spots
// in the board coordinate space.  The faces do not overlap.
// Coordinates on an individual face are represented locally as rows
// and columns in the range [0, side).  The face is identified by a
// row and column that have been scaled by side.  Basically
//    board coords = side * face coords + local coords
// In the above, face coords refers to numbers up to ~3-5 where side
// has been divided out.  However there's a 1-offset for board
// coordinates to better fit the description of the problem.

// board coordinates => face coorindates
pair<coord, coord> face_coord(coord const &rc, int side) {
  auto [r, c] = rc;
  coord face_rc{ (r - 1) / side, (c - 1) / side };
  coord local_rc{ (r - 1) % side, (c - 1) % side };
  return { face_rc, local_rc };
}

// board coorindates <= face coordinates
coord board_coord(coord const &face_rc, coord const &local_rc, int side) {
  return side * face_rc + local_rc + coord({ 1, 1 });
}

// A face of the cube, oriented as on the initial board
struct face {
  // Length of cube side
  int const side;
  // Coordinates on the board (independent of side, typically 0, 1, ..., ~4)
  coord const face_rc;
  // Adjacent faces, and the edge numbers of those faces that this
  // face connects to.  Edge numbers are relative to the full board
  // and with the same orientation as the facing index:
  //   +--3--+
  //   |     |
  //   2     0
  //   |     |
  //   +--1--+
  // So, e.g., if this is face f1 and there's a face f2 just to the
  // right of this one on the initial board, then adj[0] == { f2, 2 }
  // and f2->adj[2] = { f1, 0 }.
  pair<face *, int> adj[4];

  face(int side_, coord const &face_rc_);

  // Attach adj[edge] to { f, fedge }
  void link(int edge, face *f, int fedge);

  // Try to fold up into a cube.  Sweeps around adj, looking for this
  // situation:
  //   +-----+-----+
  //   |     |     |
  //   | f2  |  f  |
  //   |     |     |
  //   +-----+-----+
  //         |     |
  //         |  f1 |
  //         |     |
  //         +-----+
  // Adjacent cyclic faces f1 and f2 must be linked along their inner
  // edges.  The inner edge on f1 is (the edge in adj for f1) - 1
  // (interpreted cyclically of course).  The inner edge on f2 is (the
  // edge in adj for f2) + 1.  num_folded is incremented for every
  // edge of f that is filled in.
  void fold(int &num_folded) const;

  // Convert between local face coordinates (ranging in [0, side)) and
  // single coordinate for edges.  Edge coordinates are [0, side),
  // with 0 being on the left when facing the edge.  E.g., edge 1 (the
  // bottom) is for local coordinates { side - 1, 0 ... side - 1 }.
  // The constant side - 1 is dropped.  The second coordinate is
  // reversed for that edge, since the bottom right corner of the face
  // is on the left when facing the bottom.
  int to_edge(coord const &local_rc, int edge) const;
  coord from_edge(int edge_coord, int edge) const;

  // If board coordinates rc are on this face, then wrap when moving
  // ahead while facing the indicated direction, update the coordinate
  // and heading, and return true.  Return false (and leave rc and
  // facing unchanged) if rc is not on this face.
  bool wrap(coord &rc, int &facing) const;

  // Wrap from an incoming edge and convert to board coordinates,
  // update facing for the direction on the board
  coord wrap_incoming(int edge, int edge_coord, int &facing) const;
};

face::face(int side_, coord const &face_rc_) : side(side_), face_rc(face_rc_) {
  for (int i = 0; i < 4; ++i)
    adj[i] = { nullptr, -1 };
}

void face::link(int edge, face *f, int fedge) {
  if (!adj[edge].first) {
    adj[edge] = { f, fedge };
  }
  assert(adj[edge].first == f && adj[edge].second == fedge);
}

void face::fold(int &num_folded) const {
  for (int e1 = 0; e1 < 4; ++e1) {
    int e2 = (e1 + 1) % 4;
    if (adj[e1].first && adj[e2].first) {
      auto [f1, ef1] = adj[e1];
      auto [f2, ef2] = adj[e2];
      int ef1_prev = (ef1 + 3) % 4;
      int ef2_next = (ef2 + 1) % 4;
      f1->link(ef1_prev, f2, ef2_next);
      f2->link(ef2_next, f1, ef1_prev);
    }
    if (adj[e1].first)
      ++num_folded;
  }
}

bool face::wrap(coord &rc, int &facing) const {
  auto [expected_face_rc, local_rc] = face_coord(rc, side);
  if (expected_face_rc != face_rc)
    // Not this face
    return false;
  // Get coordinate on the edge
  int edge_coord = to_edge(local_rc, facing);
  // Reverse when crossing to the other face
  edge_coord = side - 1 - edge_coord;
  // Delegate to the other face for updating
  auto [adjf, adjfedge] = adj[facing];
  rc = adjf->wrap_incoming(adjfedge, edge_coord, facing);
  return true;
}

coord face::wrap_incoming(int edge, int edge_coord, int &facing) const {
  coord local_rc = from_edge(edge_coord, edge);
  coord rc = board_coord(face_rc, local_rc, side);
  // Facing opposite the edge
  facing = (edge + 2) % 4;
  return rc;
}

int face::to_edge(coord const &local_rc, int edge) const {
  assert(0 <= edge && edge < 4);
  switch (edge) {
  case 0:  return local_rc.first;
  case 1:  return side - 1 - local_rc.second;
  case 2:  return side - 1 - local_rc.first;
  default: return local_rc.second;
  }
}

coord face::from_edge(int edge_coord, int edge) const {
  assert(0 <= edge && edge < 4);
  switch (edge) {
  case 0:  return { edge_coord, side - 1 };
  case 1:  return { side - 1, side - 1 - edge_coord };
  case 2:  return { side - 1 - edge_coord, 0 };
  default: return { 0, edge_coord };
  }
}

struct board {
  // Padded on all sides with with at least one space or blank line
  // for wrapping checks.  Also effectively converts to a 1-based
  // coordinate system
  vector<string> rows;
  // Starting point is (1, starting_col)
  int starting_col;
  // If true, wrap on the cube, else simple wrapping for part 1
  bool cube_wrapping;
  // Cube side length
  int side;
  // Topological information of the folded cube
  vector<face> faces;

  // Construct from cin;
  board(bool cube_wrapping_);

  // Find topological information for the cube
  void fold();

  // What's at a particular location?  Space means wrapping is needed
  char at(coord const &rc) const;

  // Try a single step, return true if moved, false if blocked
  bool step(coord &rc, int &facing) const;
};

board::board(bool cube_wrapping_) : cube_wrapping(cube_wrapping_) {
  rows.push_back("");
  string line;
  int width = 0;
  while (getline(cin, line) && line != "") {
    width = max(width, int(line.length()));
    line.push_back(' ');
    rows.push_back(' ' + line);
  }
  rows.push_back("");
  int height = rows.size() - 2;
  // Compute cube information
  side = gcd(width, height);
  starting_col = rows[1].find('.');
  for (int r = 1; r <= height; r += side)
    for (int c = 1; c <= width; c += side)
      if (at({ r, c }) != ' ')
        faces.emplace_back(side, face_coord({ r, c }, side).first);
  fold();
}

void board::fold() {
  assert(faces.size() == 6);
  // Horizontal and vertical direct links
  for (auto &f1 : faces)
    for (auto &f2 : faces) {
      if (f2.face_rc - f1.face_rc == coord({ 0, +1 })) {
        // f2 to the right of f1
        f1.link(0, &f2, 2);
        f2.link(2, &f1, 0);
      }
      if (f2.face_rc - f1.face_rc == coord({ -1, 0 })) {
        // f2 above f1
        f1.link(3, &f2, 1);
        f2.link(1, &f1, 3);
      }
    }
  // Fold repeatedly until all edge links are identified
  int num_folded;
  do {
    num_folded = 0;
    for (auto &f : faces)
      f.fold(num_folded);
  } while (num_folded < 6 * 4);
}

char board::at(coord const &rc) const {
  auto [r, c] = rc;
  if (r < 0 || c < 0 || r >= int(rows.size()) || c >= int(rows[r].length()))
    return ' ';
  return rows[r][c];
}

bool board::step(coord &rc, int &facing) const {
  int dc = facing == 0 ? +1 : (facing == 2 ? -1 : 0);
  int dr = facing == 1 ? +1 : (facing == 3 ? -1 : 0);
  coord delta(dr, dc);
  coord rcnext = rc + delta;
  int facingnext = facing;
  char ahead = at(rcnext);
  if (ahead == ' ') {
    // Wrap
    if (!cube_wrapping)
      while (at(rcnext - delta) != ' ')
        rcnext = rcnext - delta;
    else {
      rcnext = rc;
      for (auto const &f : faces)
        if (f.wrap(rcnext, facingnext))
          break;
    }
    ahead = at(rcnext);
  }
  if (ahead == '#')
    // Blocked
    return false;
  // Move forward
  rc = rcnext;
  facing = facingnext;
  return true;
}

void simulate(bool cube_wrapping) {
  board b(cube_wrapping);
  string path;
  getline(cin, path);
  assert(cin);
  coord rc(1, b.starting_col);
  int facing = 0;
  for (size_t i = 0; i < path.length(); ) {
    b.rows[rc.first][rc.second] = ">v<^"[facing];
    char instr = path[i];
    if (instr == 'L' || instr == 'R') {
      facing = (facing + (instr == 'L' ? 3 : 1)) % 4;
      ++i;
    } else {
      assert(isdigit(instr));
      int num_steps = atoi(path.c_str() + i);
      while (isdigit(path[++i]))
        ;
      while (num_steps-- > 0 && b.step(rc, facing))
        b.rows[rc.first][rc.second] = ">v<^"[facing];
    }
  }
  cout << 1000 * rc.first + 4 * rc.second + facing << '\n';
}

void part1() { simulate(false); }
void part2() { simulate(true); }

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

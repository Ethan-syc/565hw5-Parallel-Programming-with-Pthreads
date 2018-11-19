//
// Created by Yichuan Shi on 11/16/18.
//

#include "utility.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

void initializeGridElevation(Grid<Node> &grid, string file_path) {
  ifstream inputFIle(file_path);
  if (inputFIle.fail()) {
    cout << "fail doesn't exist" << endl;
    exit(EXIT_FAILURE);
  }
  string line;
  for (size_t i = 1; i < N + 1; i++) {
    getline(inputFIle, line);
    stringstream line_stream(line);
    for (size_t j = 1; j < N + 1; j++) {
      line_stream >> grid[i][j].elevation;
    }
  }
}

void initializeGridTrickle(Grid<Node> &grid) {
  for (int i = 1; i < N + 1; ++i) {
    for (int j = 1; j < N + 1; ++j) {
      initializeNodeTrickle(grid, i, j);
    }
  }
}

void initializeNodeTrickle(Grid<Node> &grid, int i, int j) {
  vector<int> neighbors_elevation(0);
  neighbors_elevation.push_back(grid[i - 1][j].elevation);
  neighbors_elevation.push_back(grid[i + 1][j].elevation);
  neighbors_elevation.push_back(grid[i][j - 1].elevation);
  neighbors_elevation.push_back(grid[i][j + 1].elevation);
  int lowest_elevation =
          *min_element(neighbors_elevation.begin(), neighbors_elevation.end());
  if (lowest_elevation >= grid[i][j].elevation) {
    grid[i][j].willTrickle = false;
    return;
  }
  grid[i][j].willTrickle = true;
  int num_of_lowest_elevation = 0;
  for (int neighbor_elevation : neighbors_elevation) {
    if (neighbor_elevation == lowest_elevation) {
      num_of_lowest_elevation++;
    }
  }
  double each_trickle_amount = 1.0 / num_of_lowest_elevation;
  if (lowest_elevation == grid[i - 1][j].elevation) {
    grid[i][j].topTrickle = each_trickle_amount;
  }
  if (lowest_elevation == grid[i + 1][j].elevation) {
    grid[i][j].bottomTrickle = each_trickle_amount;
  }
  if (lowest_elevation == grid[i][j - 1].elevation) {
    grid[i][j].leftTrickle = each_trickle_amount;
  }
  if (lowest_elevation == grid[i][j + 1].elevation) {
    grid[i][j].rightTrickle = each_trickle_amount;
  }
}

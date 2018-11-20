
#include "Node.h"
#include "Grid.h"
#include "utility.h"
#include <chrono>

using namespace std;
int M;
double A;
int N;

bool checkIfAllDry(const Grid<Node> &grid) {
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      if (grid[i][j].current > 0) {
        return false;
      }
    }
  }
  return true;
}

int simulate(Grid<Node> &grid) {
  int time_steps = 0;
  while (time_steps <= M || !checkIfAllDry(grid)) {
    for (size_t i = 1; i < N + 1; i++) {
      for (size_t j = 1; j < N + 1; j++) {
        if (time_steps < M) {
          grid[i][j].current++;
        }
        if (grid[i][j].current > 0) {
          double absorb_amount =
                  (grid[i][j].current > A) ? A : grid[i][j].current;
          grid[i][j].current -= absorb_amount;
          grid[i][j].absorbed += absorb_amount;
        }
        if (grid[i][j].current > 0 && grid[i][j].willTrickle) {
          double total_amount_to_trickle =
                  (grid[i][j].current >= 1) ? 1 : grid[i][j].current;
          grid[i][j].trickleAmount = total_amount_to_trickle;
          grid[i][j].current -= total_amount_to_trickle;
        } else {
          grid[i][j].trickleAmount = 0;
        }
      }
    }
    for (size_t i = 1; i < N + 1; i++) {
      for (size_t j = 1; j < N + 1; j++) {
        if (grid[i][j].willTrickle) {
          grid[i - 1][j].current += grid[i][j].topTrickle * grid[i][j].trickleAmount;
          grid[i + 1][j].current += grid[i][j].bottomTrickle * grid[i][j].trickleAmount;
          grid[i][j - 1].current += grid[i][j].leftTrickle * grid[i][j].trickleAmount;
          grid[i][j + 1].current += grid[i][j].rightTrickle * grid[i][j].trickleAmount;
        }
      }
    }
    time_steps++;
  }
  return time_steps;
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    cout << "usage: ./rainfall <M> <A> <N> <elevation_file>" << endl;
    exit(EXIT_FAILURE);
  }
  M = atoi(argv[1]); // time stamp
  A = atof(argv[2]); // absorb rate
  N = atoi(argv[3]); // grid size
  Grid<Node> grid(N + 2);
  initializeGridElevation(grid, argv[4]);
  initializeGridTrickle(grid);
  auto t1 =
          chrono::time_point_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now())
                  .time_since_epoch()
                  .count();
  int time_steps =
          simulate(grid);

  auto t2 =
          chrono::time_point_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now())
                  .time_since_epoch()
                  .count();
  cout << "Rainfall simulation completed in " << time_steps
       << " time steps" << endl;
  cout << "Runtime = " << (t2 - t1) / 1e9 << " seconds" << endl
       << endl;
  cout << "The following grid shows the number of raindrops absorbed at each "
          "point : "
       << endl;
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      cout << grid[i][j].absorbed << " ";
    }
    cout << endl;
  }
}


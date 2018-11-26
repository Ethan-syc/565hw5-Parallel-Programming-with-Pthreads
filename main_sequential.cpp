#include "Node.h"
#include "Grid.h"
#include "utility.h"
#include <chrono>
#include <algorithm>

int simulate(Grid<Node> &grid, const int &N, const int &M, const float &A);

void rainAndAbsorb(Grid<Node> &grid, const int &time_steps, const int &N, const int &M, const float &A);

bool checkAllDry(Grid<Node> &grid, int &time_steps, const int &N, const float &A);

void trickle(Grid<Node> &grid, const int &N);

using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 6) {
    cout << "usage: ./rainfall <P> <M> <A> <N> <elevation_file>" << endl;
    exit(EXIT_FAILURE);
  }
  int M = atoi(argv[2]);   // time stamp
  float A = atof(argv[3]); // absorb rate
  int N = atoi(argv[4]);   // grid size
  Grid<Node> grid(N + 2);
  initializeGridElevation(grid, argv[5], N);
  initializeGridTrickle(grid, N);
  auto t1 =
          chrono::time_point_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now())
                  .time_since_epoch()
                  .count();
  int time_steps =
          simulate(grid, N, M, A);

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

int simulate(Grid<Node> &grid, const int &N, const int &M, const float &A) {
  int time_steps = 0;
  bool all_dry = false;
  while (!all_dry) {
    rainAndAbsorb(grid, time_steps, N, M, A);
    trickle(grid, N);
    time_steps++;
    all_dry = checkAllDry(grid, time_steps, N, A);
  }
  return time_steps;
}

void rainAndAbsorb(Grid<Node> &grid, const int &time_steps, const int &N, const int &M, const float &A) {
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      if (time_steps < M) {
        grid[i][j].current++;
      }
      if (grid[i][j].current > 0) {
        float absorb_amount =
                (grid[i][j].current > A) ? A : grid[i][j].current;
        grid[i][j].current -= absorb_amount;
        grid[i][j].absorbed += absorb_amount;
      }
      if (grid[i][j].current > 0 && grid[i][j].willTrickle) {
        float total_amount_to_trickle =
                (grid[i][j].current >= 1) ? 1 : grid[i][j].current;
        grid[i][j].trickleAmount = total_amount_to_trickle;
        grid[i][j].current -= total_amount_to_trickle;
      } else {
        grid[i][j].trickleAmount = 0;
      }
    }
  }
}

void trickle(Grid<Node> &grid, const int &N) {
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      if (grid[i][j].willTrickle) {
        float each_trickleAmount = (grid[i][j].trickleAmount / grid[i][j].trickleNumber);
        if (grid[i][j].topTrickle) {
          grid[i - 1][j].current += each_trickleAmount;
        }
        if (grid[i][j].bottomTrickle) {
          grid[i + 1][j].current += each_trickleAmount;
        }
        if (grid[i][j].leftTrickle) {
          grid[i][j - 1].current += each_trickleAmount;
        }
        if (grid[i][j].rightTrickle) {
          grid[i][j + 1].current += each_trickleAmount;
        }
      }
    }
  }
}

bool checkAllDry(Grid<Node> &grid, int &time_steps, const int &N, const float &A) {
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      if (grid[i][j].trickleAmount != 0) {
        return false;
      }
    }
  }
  int max_time_to_end_absorb = 0;
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      max_time_to_end_absorb =
              max(max_time_to_end_absorb, (int) (grid[i][j].current / A));
      grid[i][j].absorbed += grid[i][j].current;
    }
  }
  time_steps += max_time_to_end_absorb;
  return true;
}


#include "Node.h"
#include "Grid.h"
#include "utility.h"
#include <chrono>

int simulate(Grid<Node> &grid, const int& N, const int& M, const double& A) ;

bool rainAndAbsorb(Grid<Node> &grid, const int &time_steps, const int& N, const int &M, const double& A) ;

void trickle(Grid<Node> &grid, const int &N) ;

using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 5) {
    cout << "usage: ./rainfall <M> <A> <N> <elevation_file>" << endl;
    exit(EXIT_FAILURE);
  }
  int M = atoi(argv[1]); // time stamp
  double A = atof(argv[2]); // absorb rate
  int N = atoi(argv[3]); // grid size
  Grid<Node> grid(N + 2);
  initializeGridElevation(grid, argv[4], N);
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
int simulate(Grid<Node> &grid, const int& N, const int& M, const double& A) {
  int time_steps = 0;
  bool not_dry = false;
  while (time_steps <= M || not_dry) {
    not_dry = rainAndAbsorb(grid, time_steps, N, M, A);
    trickle(grid, N);
    time_steps++;
  }
  return time_steps;
}

bool rainAndAbsorb(Grid<Node> &grid, const int &time_steps, const int& N, const int &M, const double& A) {
  bool not_dry = false;
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
        not_dry = true;
      } else {
        grid[i][j].trickleAmount = 0;
      }
      if (grid[i][j].current != 0) {
        not_dry = true;
      }
    }
  }
  return not_dry;
}

void trickle(Grid<Node> &grid, const int &N) {
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      if (grid[i][j].willTrickle) {
        double each_trickleAmount = (grid[i][j].trickleAmount / grid[i][j].trickleNumber);
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



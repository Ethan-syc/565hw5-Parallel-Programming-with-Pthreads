
#include "Node.h"
#include "Grid.h"
#include "utility.h"
#include "simulation.h"


using namespace std;
int M;
double A;
int N;

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
          chrono::time_point_cast<chrono::nanoseconds>(chrono::system_clock::now())
                  .time_since_epoch()
                  .count();
  simDuringRainDrop(grid);
  int time_to_dry_after_rain_stop =
          simAfterRainDrop(grid);

  auto t2 =
          chrono::time_point_cast<chrono::nanoseconds>(chrono::system_clock::now())
                  .time_since_epoch()
                  .count();
  cout << "Rainfall simulation completed in " << time_to_dry_after_rain_stop + M
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


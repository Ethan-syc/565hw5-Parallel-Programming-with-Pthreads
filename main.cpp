#include "Grid.h"
#include "Node.h"
#include "utility.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <pthread.h>
void simulate(Grid<Node> &grid, const int &N, const int &M, const float &A,
              const int &P, const int &thread_id);

bool rainAndAbsorb(Grid<Node> &grid, atomic<int> &time_steps, const int &N,
                   const int &M, const float &A, const int &thread_id,
                   const int P);

void trickle(Grid<Node> &grid, const int &N, const int &thread_id, const int P);

void *worker(void *arg);

using namespace std;
atomic<bool> global_not_dry;
atomic<int> global_time_steps;
pthread_barrier_t barrier1;
pthread_barrier_t barrier2;
pthread_barrier_t barrier3;
pthread_mutex_t mutex1;
struct threadArgs {
  int P;
  int M;
  float A;
  int N;
  Grid<Node> *grid;
  int threadId;
};

int main(int argc, char *argv[]) {
  if (argc != 6) {
    cout << "usage: ./rainfall <P> <M> <A> <N> <elevation_file>" << endl;
    exit(EXIT_FAILURE);
  }
  int P = atoi(argv[1]);   // thread number
  int M = atoi(argv[2]);   // time stamp
  float A = atof(argv[3]); // absorb rate
  int N = atoi(argv[4]);   // grid size
  Grid<Node> grid(N + 2);
  global_not_dry = false;
  global_time_steps = 0;
  initializeGridElevation(grid, argv[5], N);
  initializeGridTrickle(grid, N);
  pthread_barrier_init(&barrier1, NULL, P);
  pthread_barrier_init(&barrier2, NULL, P);
  pthread_barrier_init(&barrier3, NULL, P);
  auto t1 = chrono::time_point_cast<chrono::nanoseconds>(
                chrono::high_resolution_clock::now())
                .time_since_epoch()
                .count();
  pthread_t *threads = (pthread_t *)malloc(P * sizeof(pthread_t));
  struct threadArgs *thread_args;
  for (int i = 0; i < P; ++i) {
    thread_args = (struct threadArgs *)malloc(sizeof(struct threadArgs));
    thread_args->P = P;
    thread_args->M = M;
    thread_args->A = A;
    thread_args->N = N;
    thread_args->grid = &grid;
    thread_args->threadId = i;
    pthread_create(threads + i, nullptr, worker, (void *)(thread_args));
  }
  for (int i = 0; i < P; ++i) {
    pthread_join(threads[i], nullptr);
  }
  auto t2 = chrono::time_point_cast<chrono::nanoseconds>(
                chrono::high_resolution_clock::now())
                .time_since_epoch()
                .count();
  cout << "Rainfall simulation completed in " << global_time_steps
       << " time steps" << endl;
  cout << "Runtime = " << (t2 - t1) / 1e9 << " seconds" << endl << endl;
  cout << "The following grid shows the number of raindrops absorbed at each "
          "point : "
       << endl;
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      cout << grid[i][j].absorbed << " ";
    }
    cout << endl;
  }
  pthread_barrier_destroy(&barrier1);
  pthread_barrier_destroy(&barrier2);
  pthread_barrier_destroy(&barrier3);
  exit(0);
}

void simulate(Grid<Node> &grid, const int &N, const int &M, const float &A,
              const int &P, const int &thread_id) {
                global_not_dry = true;
  while (global_time_steps < M || global_not_dry) {

    bool thread_i_not_dry =
        rainAndAbsorb(grid, global_time_steps, N, M, A, thread_id, P);
    pthread_barrier_wait(&barrier1);

    pthread_mutex_lock(&mutex1);
    trickle(grid, N, thread_id, P);
    pthread_mutex_unlock(&mutex1);

    pthread_barrier_wait(&barrier2);
    if (thread_id == 0) {
      global_not_dry = false;
      global_time_steps++;
    }
    global_not_dry = thread_i_not_dry | global_not_dry;
    pthread_barrier_wait(&barrier3);
  }
}

bool rainAndAbsorb(Grid<Node> &grid, atomic<int> &time_steps, const int &N,
                   const int &M, const float &A, const int &thread_id,
                   const int P) {
  bool not_dry = false;
  int stop_trickle_number = 0;
  for (int i = thread_id * (N / P) + 1; i < (thread_id + 1) * (N / P) + 1;
       i++) {
    for (int j = 1; j < N + 1; j++) {
      if (time_steps < M) {
        grid[i][j].current++;
      }
      if (grid[i][j].current > 0) {
        float absorb_amount = (grid[i][j].current > A) ? A : grid[i][j].current;
        grid[i][j].current -= absorb_amount;
        grid[i][j].absorbed += absorb_amount;
      }
      if (grid[i][j].current > 0 && grid[i][j].willTrickle) {
        float total_amount_to_trickle =
            (grid[i][j].current >= 1) ? 1 : grid[i][j].current;
        grid[i][j].trickleAmount = total_amount_to_trickle;
        grid[i][j].current -= total_amount_to_trickle;
        not_dry = true;
      } else {
        grid[i][j].trickleAmount = 0;
        stop_trickle_number++;
      }
      if (grid[i][j].current != 0) {
        not_dry = true;
      }
    }
  }
  //  if (stop_trickle_number == N * N) {
  //    int max_time_to_end_absorb = 0;
  //    for (size_t i = 1; i < N + 1; i++) {
  //      for (size_t j = 1; j < N + 1; j++) {
  //        max_time_to_end_absorb = max(max_time_to_end_absorb, (int)
  //        (grid[i][j].current / A)); grid[i][j].absorbed +=
  //        grid[i][j].current;
  //      }
  //    }
  //    time_steps += max_time_to_end_absorb;
  //    return false;
  //  }
  return not_dry;
}

void trickle(Grid<Node> &grid, const int &N, const int &thread_id,
             const int P) {
  for (int i = thread_id * (N / P) + 1; i < (thread_id + 1) * (N / P) + 1;
       i++) {
    for (int j = 1; j < N + 1; j++) {
      if (grid[i][j].willTrickle) {
        float each_trickleAmount =
            (grid[i][j].trickleAmount / grid[i][j].trickleNumber);
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

void *worker(void *arg) {
  struct threadArgs thread_args = *((struct threadArgs *)arg);
  int N = thread_args.N;
  int M = thread_args.M;
  float A = thread_args.A;
  int P = thread_args.P;
  int id = thread_args.threadId;
  simulate(*thread_args.grid, N, M, A, P, id);
}

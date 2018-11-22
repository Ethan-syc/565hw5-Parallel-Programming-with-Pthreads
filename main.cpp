#include "Grid.h"
#include "Node.h"
#include "utility.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <pthread.h>
void simulate(Grid<Node> &grid, pthread_mutex_t *mutex_array, const int &N,
              const int &M, const float &A, const int &P, const int &thread_id);

void rainAndAbsorb(Grid<Node> &grid, const int &time_steps, const int &N,
                   const int &M, const float &A, const int &workload,
                   const int &thread_id);

void trickle(Grid<Node> &grid, pthread_mutex_t *&mutex_array, const int &N,
             const int &workload, const int &thread_id);

bool checkAllDry(Grid<Node> &grid, const int &N, const float &A);

int calculateWorkload(const int &N, const int &P);

void *worker(void *arg);

using namespace std;
bool all_dry;
int time_steps;
pthread_barrier_t barrier;

struct threadArgs {
  int P;
  int M;
  float A;
  int N;
  Grid<Node> *grid;
  pthread_mutex_t *mutex_array;
  int threadId;
};

int main(int argc, char *argv[]) {
  if (argc != 6) {
    cout << "usage: ./rainfall <P> <M> <A> <N> <elevation_file>" << endl;
    exit(EXIT_FAILURE);
  }
  all_dry = false;
  time_steps = 0;
  int P = atoi(argv[1]);   // thread number
  int M = atoi(argv[2]);   // time stamp
  float A = atof(argv[3]); // absorb rate
  int N = atoi(argv[4]);   // grid size
  Grid<Node> grid(N + 2);
  pthread_mutex_t *mutex_array = new pthread_mutex_t[N + 2];
  for (size_t i = 0; i < N + 2; i++) {
    pthread_mutex_init(&mutex_array[i], NULL);
  }
  pthread_barrier_init(&barrier, NULL, P);
  initializeGridElevation(grid, argv[5], N);
  initializeGridTrickle(grid, N);
  pthread_t *threads = (pthread_t *)malloc(P * sizeof(pthread_t));
  struct threadArgs *thread_args;
  auto t1 = chrono::time_point_cast<chrono::nanoseconds>(
                chrono::high_resolution_clock::now())
                .time_since_epoch()
                .count();
  for (int i = 0; i < P; ++i) {
    thread_args = (struct threadArgs *)malloc(sizeof(struct threadArgs));
    thread_args->P = P;
    thread_args->M = M;
    thread_args->A = A;
    thread_args->N = N;
    thread_args->grid = &grid;
    thread_args->mutex_array = mutex_array;
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
  cout << "Rainfall simulation completed in " << time_steps << " time steps"
       << endl;
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
  pthread_barrier_destroy(&barrier);
  for (size_t i = 0; i < N + 2; i++) {
    pthread_mutex_destroy(mutex_array + i);
  }
  delete[] mutex_array;
  free(threads);
  // free(thread_args);
  exit(0);
}

bool checkAllDry(Grid<Node> &grid, const int &N, const float &A) {
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
          max(max_time_to_end_absorb, (int)(grid[i][j].current / A));
      grid[i][j].absorbed += grid[i][j].current;
    }
  }
  time_steps += max_time_to_end_absorb;
  return true;
}

void simulate(Grid<Node> &grid, pthread_mutex_t *mutex_array, const int &N,
              const int &M, const float &A, const int &P,
              const int &thread_id) {
  int workload = calculateWorkload(N, P);
  while (!all_dry) {
    rainAndAbsorb(grid, time_steps, N, M, A, workload, thread_id);
    pthread_barrier_wait(&barrier);
    trickle(grid, mutex_array, N, workload, thread_id);
    if (thread_id == 0) {
      time_steps++;
      all_dry = checkAllDry(grid, N, A);
    }
    pthread_barrier_wait(&barrier);
  }
}

int calculateWorkload(const int &N, const int &P) {
  int min_workload = 1;
  int workload = N / P;
  if (workload < 1) {
    workload = min_workload;
  }
  if (workload * P == N) {
    return workload;
  }
  return workload + 1;
}

void rainAndAbsorb(Grid<Node> &grid, const int &time_steps, const int &N,
                   const int &M, const float &A, const int &workload,
                   const int &thread_id) {
  for (int i = thread_id * workload + 1; i < (thread_id + 1) * workload + 1;
       i++) {
    if (i >= N + 1) {
      return;
    }
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
      } else {
        grid[i][j].trickleAmount = 0;
      }
    }
  }
}

void trickle(Grid<Node> &grid, pthread_mutex_t *&mutex_array, const int &N,
             const int &workload, const int &thread_id) {
  for (int i = thread_id * workload + 1; i < (thread_id + 1) * workload + 1;
       i++) {
    if (i >= N + 1) {
      return;
    }
    for (int j = 1; j < N + 1; j++) {
      if (grid[i][j].willTrickle) {
        float each_trickleAmount =
            (grid[i][j].trickleAmount / grid[i][j].trickleNumber);
        if (grid[i][j].topTrickle) {
          pthread_mutex_lock(&mutex_array[i - 1]);
          grid[i - 1][j].current += each_trickleAmount;
          pthread_mutex_unlock(&mutex_array[i - 1]);
        }
        if (grid[i][j].bottomTrickle) {
          pthread_mutex_lock(&mutex_array[i + 1]);
          grid[i + 1][j].current += each_trickleAmount;
          pthread_mutex_unlock(&mutex_array[i + 1]);
        }
        if (grid[i][j].leftTrickle) {
          pthread_mutex_lock(&mutex_array[i]);
          grid[i][j - 1].current += each_trickleAmount;
          pthread_mutex_unlock(&mutex_array[i]);
        }
        if (grid[i][j].rightTrickle) {
          pthread_mutex_lock(&mutex_array[i]);
          grid[i][j + 1].current += each_trickleAmount;
          pthread_mutex_unlock(&mutex_array[i]);
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
  int thread_id = thread_args.threadId;
  simulate(*thread_args.grid, thread_args.mutex_array, N, M, A, P, thread_id);
  free(arg);
}

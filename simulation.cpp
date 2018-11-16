//
// Created by Yichuan Shi on 11/16/18.
//

#include "simulation.h"
#include "utility.h"


void simDuringRainDrop(Grid<Node> &grid) {
  for (int i = 0; i < M; i++) {
    simRecv(grid);
    simAbsorb(grid);
    simTrickle(grid);
  }
}

void simTrickle(Grid<Node> &grid) {
  Grid<Node> trickle_delta(N + 2);
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      if (grid[i][j].current > 0 && grid[i][j].willTrickle) {
        double total_amount_to_trickle =
                (grid[i][j].current >= 1) ? 1 : grid[i][j].current;
        trickle_delta[i - 1][j].current += grid[i][j].topTrickle * total_amount_to_trickle;
        trickle_delta[i + 1][j].current += grid[i][j].bottomTrickle * total_amount_to_trickle;
        trickle_delta[i][j - 1].current += grid[i][j].leftTrickle * total_amount_to_trickle;
        trickle_delta[i][j + 1].current += grid[i][j].rightTrickle * total_amount_to_trickle;
        grid[i][j].current -= total_amount_to_trickle;
      }
    }
  }
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      grid[i][j].current += trickle_delta[i][j].current;
    }
  }
}

void simAbsorb(Grid<Node> &grid) {
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      if (grid[i][j].current > 0) {
        double absorb_amount =
                (grid[i][j].current > A) ? A : grid[i][j].current;
        grid[i][j].current -= absorb_amount;
        grid[i][j].absorbed += absorb_amount;
      }
    }
  }
}

void simRecv(Grid<Node> &grid) {
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      grid[i][j].current++;
    }
  }
}

int simAfterRainDrop(Grid<Node> &grid) {
  int time_to_dry_after_rain_stop = 0;
  while (!checkIfAllDry(grid)) {
    simAbsorb(grid);
    simTrickle(grid);
    time_to_dry_after_rain_stop++;
  }
  return time_to_dry_after_rain_stop;
}

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



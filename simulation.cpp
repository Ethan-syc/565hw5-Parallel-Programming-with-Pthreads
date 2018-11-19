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

bool simTrickle(Grid<Node> &grid) {
  bool isAllDry = true;
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      if (grid[i][j].current > 0 && grid[i][j].willTrickle) {
        double total_amount_to_trickle =
                (grid[i][j].current >= 1) ? 1 : grid[i][j].current;
        grid[i][j].trickleAmount = total_amount_to_trickle;
        grid[i][j].current -= total_amount_to_trickle;
        isAllDry = false;
      } else {
        grid[i][j].trickleAmount = 0;
      }
    }
  }
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      grid[i - 1][j].current += grid[i][j].topTrickle * grid[i][j].trickleAmount;
      grid[i + 1][j].current += grid[i][j].bottomTrickle * grid[i][j].trickleAmount;
      grid[i][j - 1].current += grid[i][j].leftTrickle * grid[i][j].trickleAmount;
      grid[i][j + 1].current += grid[i][j].rightTrickle * grid[i][j].trickleAmount;
    }
  }
  return isAllDry;
}

bool simAbsorb(Grid<Node> &grid) {
  bool isAllDry = true;
  for (size_t i = 1; i < N + 1; i++) {
    for (size_t j = 1; j < N + 1; j++) {
      if (grid[i][j].current > 0) {
        double absorb_amount =
                (grid[i][j].current > A) ? A : grid[i][j].current;
        grid[i][j].current -= absorb_amount;
        grid[i][j].absorbed += absorb_amount;
        if (grid[i][j].current != 0) {
          isAllDry = false;
        }
      }
    }
  }
  return isAllDry;
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
  bool isAllDry = false;
  while (!isAllDry) {
    bool absorbDry = simAbsorb(grid);
    bool trickleDry = simTrickle(grid);
    time_to_dry_after_rain_stop++;
    isAllDry = absorbDry & trickleDry;
  }
  return time_to_dry_after_rain_stop;
}




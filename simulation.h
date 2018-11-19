//
// Created by Yichuan Shi on 11/16/18.
//

#ifndef RAINFALL_SIMULATION_H
#define RAINFALL_SIMULATION_H


#include "Grid.h"
#include "Node.h"

void simDuringRainDrop(Grid<Node> &grid);

int simAfterRainDrop(Grid<Node> &grid);

void simRecv(Grid<Node> &grid);

bool simAbsorb(Grid<Node> &grid);

bool simTrickle(Grid<Node> &grid);

#endif //RAINFALL_SIMULATION_H

//
// Created by Yichuan Shi on 11/16/18.
//

#ifndef RAINFALL_UTILITY_H
#define RAINFALL_UTILITY_H

#include "Grid.h"
#include "Node.h"
#include <string>

using namespace std;

// rain time
extern int M;

// absorb rate
extern float A;

// grid size
extern int N;

/**
 * read the elevation from the file
 * and fill corresponding Node's elevation field in the grid
 * @param grid
 */
void initializeGridElevation(Grid<Node> &grid, string file_path);

/**
 * pre-compute every Node's
 * topTrickle, bottomTrickle, leftTrickle, rightTrickle field in the grid
 * @param grid
 */
void initializeGridTrickle(Grid<Node> &grid);

void initializeNodeTrickle(Grid<Node> &grid, int i, int j);

#endif //RAINFALL_UTILITY_H

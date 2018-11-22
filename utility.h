//
// Created by Yichuan Shi on 11/16/18.
//

#ifndef RAINFALL_UTILITY_H
#define RAINFALL_UTILITY_H

#include "Grid.h"
#include "Node.h"
#include <string>

using namespace std;

/**
 * read the elevation from the file
 * and fill corresponding Node's elevation field in the grid
 * @param grid
 */
void initializeGridElevation(Grid<Node> &grid, const string& file_path, const int &N);

/**
 * pre-compute every Node's in the Grid
 * topTrickle, bottomTrickle, leftTrickle, rightTrickle, trickleNumber, willTrickle field in the grid
 * @param grid
 */
void initializeGridTrickle(Grid<Node> &grid, const int& N);

/**
 * helper function for every Node in initializeGridTrickle
 * @param grid
 * @param i
 * @param j
 */
void initializeNodeTrickle(Grid<Node> &grid, const int& i, const int& j);

#endif //RAINFALL_UTILITY_H

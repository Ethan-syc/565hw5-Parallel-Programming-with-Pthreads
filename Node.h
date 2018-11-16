//
// Created by Yichuan Shi on 11/16/18.
//

#ifndef RAINFALL_NODE_H
#define RAINFALL_NODE_H

#include <climits>

class Node {
public:
  int elevation;
  double current{};
  double absorbed{};
  bool willTrickle{};
  double trickleAmount{};
  double topTrickle{};
  double bottomTrickle{};
  double leftTrickle{};
  double rightTrickle{};

  Node() : elevation(INT_MAX) {}
};


#endif //RAINFALL_NODE_H

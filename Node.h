//
// Created by Yichuan Shi on 11/16/18.
//

#ifndef RAINFALL_NODE_H
#define RAINFALL_NODE_H

#include <climits>

class Node {
public:
  int elevation;
  float current{};
  float absorbed{};
  bool willTrickle{};
  float trickleAmount{};
  int trickleNumber{};
  bool topTrickle{};
  bool bottomTrickle{};
  bool leftTrickle{};
  bool rightTrickle{};

  Node() : elevation(INT_MAX) {}
};


#endif //RAINFALL_NODE_H

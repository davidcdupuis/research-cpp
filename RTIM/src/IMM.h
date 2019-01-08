#ifndef __IMM_H__
#define __IMM_H__

#include <math.h>

class IMM{
public:
  int l = 1;
  int seedSize = 50;
  double epsilon = 0.5;
  double opt_prime;
  std::vector<int> seedSet;

  Graph& graph;

  IMM(Graph& g);

  double samplingPhase();

  double nodeSelectionPhase();

  run();

  continueMenu();
}

#endif

#ifndef __INFSCORE_H__
#define __INFSCORE_H__

#include <iostream>
#include <string>

class InfScore{
public:
  int simulations;
  Graph& graph;

  InfScore(Graph& g);

  int run();

  int menu();

  int monteCarloMenu();

  int activationProbabilitiesMenu();

  double mcInfScore();

  double apInfScore();
};

#endif

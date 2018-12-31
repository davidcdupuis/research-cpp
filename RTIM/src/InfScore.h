#ifndef __INFSCORE_H__
#define __INFSCORE_H__

#include <iostream>
#include <string>
#include <unistd.h>

#include "Graph.h"
#include "Tools.h"

class InfScore{
public:
  int simulations = 10000;
  int depth = 10000;
  int minEdgeWeight = 0;
  std::vector<int> seedSet;
  Graph& graph;

  InfScore(Graph& g);

  void importSeedSet(std::string filePath);

  int run(std::string prevClass);

  int main(std::string prevClass);

  int monteCarloMenu(std::string prevClass);

  int activationProbabilitiesMenu(std::string prevClass);

  int continueMenu(std::string prevClass, std::string prevStage);

  double mcInfScore();

  double mcInfScoreParallel();

  int infSimulation(bool *visited);

  double apInfScore();

  void saveSeedScoreLog(std::string file, std::string startDate, std::string endDate, double& runtime, double& score);

  void saveSeedScoreCSV(std::string file, std::string startDate, std::string endDate, double& runtime, double& score);
};

#endif

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

  /** Function to run Monte Carlo simulations and return results
    *
    * @param values
    * @param seed_set
    * @param depth
    * @param sim
    *
    * @ return int []
    */
  void influenceScoreValues(std::vector<double>& values, const std::vector<int>& seed_set, int depth=10000, int sim=10000) const;

  /** Constant function to compute influence score of a given node
    *
    * @param node: source node for exploration
    * @param max_depth: max depth of path exploration
    * @param type: type of path we want to use for computing inf score
    * @param edge_weight: if defined, sets the edge weight
    * @param min_weight:
    *
    * @return double
    */
  double influenceScorePath(int node, int max_depth=3, std::string type="shortest", double edge_weight=-1, double min_weight=0) const;

  /** Constant function to perform random influence coverage of seed set
    *
    * @param seed_set: starting influential nodes
    * @param depth: exploration depth, default is 10_000 to mark infinity
    * @param minEdgeWeight: minimum edge weight threshold, default is 0
    *
    * @return int: number of activated nodes
    */

  /** Function to find all shortest paths weights of max_depth from a node
    *
    * @param distances
    * @param node: starting node of path
    * @param min_weight: minimum path weight to continue exploration
    * @param maxsepth: maximum path depth to continue exploration
    * @param curr_dist: current distance of path
    * @param edge_weight: if defined, sets the edge weight, value from 0 to 1
    */
  void shortestPathsWeights(std::map<int, double>& distances, int node, double min_weight, int max_depth, double curr_dist=1);

  // Constanct function to compute shortest paths
  void shortestPathsWeightsB(double* distances, int node, int max_depth, double curr_dist=1);

  /** Only use first neighbors to compute influence score
    *
    * @param node: index of node we wish to compute influence score of
    *
    * @return double, influence score
    */
  double influenceScoreNeighbors(int node);

  // Function to update the local influence based on activated neighbors
  double influenceLocalScore(int origine, std::vector<double>& scores, int node, std::vector<double>& activationProbs, int maxDepth = 100) const;

};

#endif

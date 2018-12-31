#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <set>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <omp.h>
#include <random>
#include <unistd.h>

#include "Tools.h"

/**
  * @file Graph.h
  * @author David Dupuis <david.dupuis@devinci.fr>
  * @version 3.0
  *
  * @section DESCRIPTION
  * Influence Graph class
  *
*/
class Graph
{
public:
  std::string dataset;          // name of dataset
  std::string datasetDir;       /**< directory of dataset */
  std::string infModel = "IC";         // influence model
  int nodes;                    /**< number of nodes */
  int edges;                    /**< number of edges*/
  int simulations;              // number of simulations to run for inf score
  double edgeWeight = -1;
  bool loaded = false; //true if dataset has been loaded
  std::vector<int> inDegrees;
  std::vector<int> outDegrees;
  std::vector<std::vector<std::pair<int, double> > > graph;

  Graph();

  /** Function to add an edge
    *
    * @param a: user influencee
    * @param b: user influenced
    * @param w: influence weight
    */
  void addEdge(int a, int b, double w);

  // Function to read attributes.txt
  void readAttributes();

  // Function to import graph from file
  void loadGraph();

  // Function to import node degree info
  void importDegrees();

  // Function to print graph
  void print();

  /**
    * Function to compute influence score of a seed set using Monte Carlo simulations SEQUENTIALLY
    * @param seed_set
    * @param depth
    * @param minEdgeWeight
    * @param sim
    *
    * @return double, influence score
  */
  // double influenceScore(const std::vector<int>& seed_set, int depth=10000, double minEdgeWeight=0, int sim=10000) const;

  /**
    * Function to compute influence score of a seed set using Monte Carlo simulations in PARALLEL
    * @param seed_set
    * @param depth
    * @param minEdgeWeight
    * @param sim
    *
    * @return double, influence score
  */
  // double influenceScoreParallel(const std::vector<int>& seed_set, int depth=10000, double minEdgeWeight=0, int sim=10000) const;

  /**
    * Function to run Monte Carlo simulations and return results
    *
    * @param values
    * @param seed_set
    * @param depth
    * @param sim
    *
    * @ return int []
    */
  void influenceScoreValues(std::vector<double>& values, const std::vector<int>& seed_set, int depth=10000, int sim=10000) const;

  /**
    * Constant function to compute influence score of a given node
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

  /**
    * Constant function to perform random influence coverage of seed set
    *
    * @param seed_set: starting influential nodes
    * @param depth: exploration depth, default is 10_000 to mark infinity
    * @param minEdgeWeight: minimum edge weight threshold, default is 0
    *
    * @return int: number of activated nodes
  */
  // int influenceSimulation(const std::vector<int>& seed_set, bool *visited, int depth=10000, double minEdgeWeight=0) const;

  /**
    * Function to find all shortest paths weights of max_depth from a node
    *
    * @param distances
    * @param node: starting node of path
    * @param min_weight: minimum path weight to continue exploration
    * @param maxsepth: maximum path depth to continue exploration
    * @param curr_dist: current distance of path
    * @param edge_weight: if defined, sets the edge weight, value from 0 to 1
    */
  void shortestPathsWeights(std::map<int, double>& distances, int node, double min_weight, int max_depth, double curr_dist=1) const;

  /**
    * Constanct function to compute shortest paths
    */
  void shortestPathsWeightsB(double* distances, int node, int max_depth, double curr_dist=1) const;

  /**
    * Only use first neighbors to compute influence score
    *
    * @param node: index of node we wish to compute influence score of
    *
    * @return double, influence score
    */
  double influenceScoreNeighbors(int node) const;

  // Function to update the local influence based on activated neighbors
  double influenceLocalScore(int origine, std::vector<double>& scores, int node, std::vector<double>& activationProbs, int maxDepth = 100) const;

  // Update activation probabilities of neighboring nodes
  void updateNeighborsAP(int src, std::vector<double>& activationProbs, std::set<int> path, double path_weight=1, int depth=2) const;

  /**
    * Update activation probabilies of neighboring nodes at depth 1
    *
    * @param src:
    * @param activationProbs:
    */
  void updateNeighborsAPShort(int src, std::vector<double>& activationProbs) const;

  /** Depth first recursive function to update neighbor's AP
    * We don't visit a neighbors who's AP = 1
    */
  void updateNeighborsAPDepth(int src, std::vector<double>& activationProbs, int maxDepth = 100) const;

  void updateInfluenceScore(double &infScore, int src, std::vector<double>& activationProbs, std::vector<double>& tmpAPs, int depth=2) const;
};
#endif

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
};
#endif

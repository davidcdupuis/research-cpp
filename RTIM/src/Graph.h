#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <set>

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
    std::string dataset; // stores dataset name for graph
    int nodes; // nodes
    int edges; // edges
    std::vector<std::vector<std::pair<int, double> > > graph;

    Graph(std::string d, bool import);


    /**
      * Function to add an edge
      */
    void addEdge(int a, int b, double w);


    /**
      * Function to read attributes.txt
      */
    void readAttributes();


    /**
      * Function to import graph from file
      */
    void loadGraph();


    /**
      * Function to compute influence score of a seed set
      *
      * @return double, influence score
    */
    double influenceScore(const std::vector<int>& seed_set, int depth=10000, int sim=10000) const;


    /**
      * Constanct function to compute influence score of a given node
      *
      * @param node: source node for exploration
      * @param max_depth: max depth of path exploration
      * @param type: type of path we want to use for computing inf score
      * @param edge_weight: if defined, sets the edge weight
      *
      * @return double
      */
    double influenceScorePath(int node, int max_depth=3, std::string type="shortest", double edge_weight=-1) const;


    /**
      * Constant function to perform random influence coverage of seed set
      *
      * @param seed_set: starting influential nodes
      * @param depth: default depth is "infinite"
      *
      * @return int: number of activated nodes
    */
    int influenceSimulation(const std::vector<int>& seed_set, int depth=10000) const;


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
    void shortestPathsWeights(std::map<int, double>& distances, int node, double min_weight, int max_depth, double curr_dist=1, double edge_weight=-1) const;


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


    /**
      * Update activation probabilities of neighboring nodes
      */
    void updateNeighborsAP(int src, std::vector<double>& activationProbs, std::set<int> path, double path_weight=1, int depth=2) const;


    /**
      * Update activation probabilies of neighboring nodes at depth 1
      */
    void updateNeighborsAPShort(int src, std::vector<double>& activationProbs) const;


    /**
      * Function to print graph
      */
    void print();
};
#endif

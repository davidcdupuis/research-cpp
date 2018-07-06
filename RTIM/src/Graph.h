#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <set>

/*
  Influence Graph class
*/
class Graph
{
  public:
    std::string dataset; // stores dataset name for graph
    int nodes; // nodes
    int edges; // edges
    std::vector<std::vector<std::pair<int, double> > > graph;

    Graph(std::string d, bool import);

    /* Function to add an edge */
    void addEdge(int a, int b, double w);

    /* Function to read attributes.txt */
    void readAttributes();

    /* Function to import graph from file */
    void loadGraph();

    /* Function to compute influence score of a seed set
       Returns influence score
    */
    double influenceScore(const std::vector<int>& seed_set, int depth=10000, int sim=10000) const;

    /* Function to compute influence score of a given node */
    double influenceScorePath(int node, int max_depth=3, std::string type="shortest") const;

    /* Function to perform random influence coverage of seed set
       Returns number of activated nodes
    */
    int influenceSimulation(const std::vector<int>& seed_set, int depth=10000) const;

    /* Function to find all shortest paths weights of max_depth from a node */
    void shortestPathsWeights(std::map<int, double>& distances, int node, int max_depth, double curr_dist=1) const;

    void shortestPathsWeightsB(double* distances, int node, int max_depth, double curr_dist=1) const;

    /* Only use first neighbors to compute influence score */
    double influenceScoreNeighbors(int node) const;

    /* Update activation probabilities of neighboring nodes */
    void updateNeighborsAP(int src, std::vector<double>& activationProbs, std::set<int> path, double path_weight=1, int depth=2) const;

    /* Function to print graph */
    void print();
};
#endif

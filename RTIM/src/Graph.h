#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <string>
#include <vector>
#include <utility>

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

    Graph(std::string d);

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

    /* Function to compute influence score of a node
    double influenceScore(int node, int depth=10000, int sim=10000) const;*/

    /* Function to perform random influence coverage of seed set
       Returns number of activated nodes
    */
    int influenceSimulation(const std::vector<int>& seed_set, int depth=10000) const;

    /* Function to print graph */
    void print();
};
#endif

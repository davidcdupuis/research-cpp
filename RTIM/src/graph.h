#define HEAD_INFO
//#define HEAD_TRACE
#include "sfmt/SFMT.h"
#include <vector>
#include "head.h"
// #include <boost/graph/adjacency_list.hpp>
using namespace std;

// typedef boost::adjacency_list<vecS, vecS, directedS> Mygraph;
// typedef property<edge_weight_t, double> EdgeWeightProperty;

/*
  Influence Graph class
*/
class Graph
{
  public:
    string dataset; // stores dataset name for graph
    int nodes; // nodes
    int edges; // edges
    vector<vector<pair<int, double>>> graph;

    Graph(string d);

    /* Function to add an edge */
    void addEdge(int a, int b, double w);

    /* Function to read attributes.txt */
    void readAttributes();

    /* Function to import graph from file */
    void loadGraph();

    /* Function to compute influence score of a seed set */
    double influenceScore();

    /* Function to perform random influence coverage of seed set */
    int influenceCoverage();

    /* Function to print graph */
    void print();
};

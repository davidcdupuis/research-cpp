#ifndef __RTIM_H__
#define __RTIM_H__

#include <string>
#include <vector>
#include "Graph.h"

/*RTIM = Real-Time Influence Maximization

  Step1: Pre_process graph by computing influence score of all users
  Step2: Dynamically decide user targeting and save to seed set
*/
class RTIM{
public:
  int numNodes;
  std::string dataset;   //dataset name
  int k;            //seed set size limit
  int simulations;  // # simulations for inf score
  int max_depth;    //max exploration depth for inf score
  std::vector<double> infScores;

  RTIM(std::string dataset);

  /*Pre-processing stage
    Compute influence score of every user in graph
  */
  void pre_process(const Graph& graph);

  /*Live stage
    For stream of user decide to target, if targeted update activation
    probability of neighboring nodes
  */
  void live(const Graph& graph);

  /* Print influence scores */
  void printScores();

};
#endif

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
  std::vector<int> seedSet; // users to target
  std::vector<double> activationProbabilities; // array of activation probabilities

  RTIM(std::string dataset);

  /*Pre-processing stage
    Compute influence score of every user in graph
  */
  void pre_process(const Graph& graph, int max_depth=100000);

  /*Live stage
    For stream of user decide to target, if targeted update activation
    probability of neighboring nodes
  */
  void live(const Graph& graph, int max_size);

  /* Import live stream */
  void importLiveStream();

  /* Print influence scores */
  void printScores();

  /* Saves influence scores to file */
  void saveScores();

  /* Import influence scores */
  void importScores();

  /* Save influence scores */
  void saveToCSV(std::string fileName);

  /* Print progress */
  int print_progress(int nb_threads, int finishedProcess, int numNodes, time_t startTime, int* nb_nodes, int save);

  /* Generate availability stream */
  void availabilityStream(std::string model, int version, int size);

};
#endif

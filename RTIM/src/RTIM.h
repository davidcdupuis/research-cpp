#ifndef __RTIM_H__
#define __RTIM_H__

#include <string>
#include <vector>

#include "Graph.h"
#include "Arguments.h"


/**
  * @file RTIM.h
  * @author David Dupuis <david.dupuis@devinci.fr>
  * @version 3.0
  *
  * @section DESCRIPTION
  *
  * RTIM = Real-Time Influence Maximization
  *
  * Step1: Pre_process graph by computing influence score of all users
  * Step2: Dynamically decide user targeting and save to seed set
  *
  *
  *
  *
*/
class RTIM{
public:
  int nodes;                         /**< number of nodes in graph */
  int simulations;                   /**< # simulations for inf score */
  int infIndex;                      /**< index of influence score threshold */

  std::vector<double> infScores;     /**< array of influence scores */
  std::vector<double> sortedScores;  /**< array of sorted influence scores */
  std::vector<int> seedSet;          /**< users to target */
  std::vector<double> activationProbabilities; /**< array of activation probabilities*/
  Arguments args;                    /**< command line arguments */
  const Graph graph;

  /**
    * Constructor
    *
    * @param dataset: name of dataset to use
    */
  RTIM(Arguments& arguments, bool loadGraph);


  /**
    * Pre-processing stage
    * Compute influence score of every user in graph
    *
    * @param graph: user influence network to use
    * @param max_depth: maximum depth for inf. score computation
    */
  void pre_process();//const Graph& graph);


  /**
    * Live stage
    * For stream of user decide to target, if targeted update activation
    * probability of neighboring nodes
    *
    * @param graph
    * @param max_size
    * @param model
    * @param version
    * @param size
    * @param ap
    * @âram infReach
    *
    */
  void live();


  /**
    * Import live stream
    */
  void importLiveStream();


  /**
    * Print influence scores
    */
  void printScores();

  /**
    * Saves influence scores to appropriate file
    */
  void saveScores();

  /**
    * Save seed set to appropriate file
    */
  void saveSeedSet();

  /**
    * Save live log data to file
    *
    * @param score
    * @param streamTime
    * @param seedTime
    * @param maxTime
    * @param progress
    * @param runtime
    */
  void saveLiveLog(double& score, double& streamTime, double& seedTime, double& maxTime, int& progress, double& runtime);


  /**
    * Save live data to CSV file
    *
    * @param graph
    * @param score
    * @param streamTime
    * @param seedTime
    * @param maxTime
    * @param progress
    * @param runtime
    */
  void saveLiveCSV(const Graph& graph, double& score, double& streamTime, double& seedTime, double& maxTime, int& progress, double& runtime);


  /**
    * Import influence scores
    */
  void importScores();

  /**
    * Save influence scores
    *
    * @param fileName
    */
  void saveToCSV(std::string fileName);

  /**
    * Print progress
    *
    * @param nb_threads
    * @param finishedProcess
    * @param numNodes
    * @param startTime
    * @param nb_nodes
    * @param save
    */
  int print_progress(int nb_threads, int finishedProcess, int numNodes, time_t startTime, int* nb_nodes, int save);


  /**
    * Generate availability stream
    * NOT IMPLEMENTED
    *
    * @param model
    * @param version
    * @param size
    */
  void availabilityStream();


  /**
    * Computes the influence score threshold index based on reach
    *
    * @param sorted: array of sorted influence scores
    */
  void getInfIndex(std::vector<double> & sorted);

};
#endif

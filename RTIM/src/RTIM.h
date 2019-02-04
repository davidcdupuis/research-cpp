#ifndef __RTIM_H__
#define __RTIM_H__

#include <string>
#include <vector>
#include <math.h>
#include <cstdio>
#include <ctime>
#include <time.h>
#include <omp.h>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <stdexcept>

#include "Graph.h"
#include "Tools.h"
#include "InfScore.h"


/**
  * @file RTIM.h
  * @author David Dupuis <david.dupuis@devinci.fr>
  * @version 3.1
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
  std::map<std::string, std::map<std::string, std::string> > datasets;
  std::map<std::string, std::string> keyword = {
    { "infScores", "infS"},
    { "seedSet", "ss"},
    { "stream", "st"},
    { "uniform_rand_repeat", "urr"},
    { "uniform_rand_no_repeat", "urnr"},
    { "inNOut_repeat", "inoutr"},
    { "progress", "prg"},
  };
  bool loadScores = true; // if false don't load scores
  bool useIMM = false;
  // int nodes;                         /**< number of nodes in graph */
  // int simulations;                   /**< # simulations for inf score */
  int infIndex;                      /**< index of influence score threshold */
  int maxSeedSize = 200;               /**< size of seed set if defined */
  int streamVersion = 1;               /**< version of stream */
  int streamSize = -1;                 /**< size of stream to use*/
  int maxDepth = 2;                    /**< max depth for monte carlo simulations */

  double minEdgeWeight = 0;              /**< minimum weight to continue influence propagation*/
  double reach = 1;                       /**< reach to define inf. threshold = percentage */
  double theta_ap = 0.8;              /**< activation probability threshold [0, 1] */

  std::string seedSetPath;
  std::string streamModel = "inNOut_repeat"; /**< name of availability model */
  std::string stage;                  /**< pre|live|evaluate */

  std::vector<double> infScores;     /**< array of influence scores */
  std::vector<double> nodeTime;      /**< time took to compute node inf score */
  std::vector<double> sortedScores;  /**< array of sorted influence scores */

  std::vector<int> seedSet;          /**< users to target */
  std::vector<int> immTargeted;
  std::vector<int> immSeedSet;
  std::string seedFile = "";
  std::vector<double> activationProbabilities; /**< array of activation probabilities*/
  std::vector<double> preActProbs; // preProcessed activation probabilities
  Graph& graph;

  RTIM(Graph& g);

  // PRE-PROCESSING FUNCTIONS

  /** Pre-processing stage
    * Compute influence score of every user in graph
    *
    * @param graph: user influence network to use
    * @param max_depth: maximum depth for inf. score computation
    */
  void infScorePreProcess();

  void actProbPreProcess();

  void saveActProbs();

  void importActProbs();

  // void updateNeighborsAP(int src, vector<double>& activationProbs, set<int> path, double path_weight, int depth);

  void initializeInfluenceScores();

  // Print influence scores
  void printScores();

  // Saves influence scores to appropriate file
  void saveScores();

  void printStageParams();

  // LIVE FUNCTIONS

  void liveExploration(int& sum, int currUser, double currPathWeight, bool activated, int depth=2);

  /** Live stage
    * For stream of user decide to target, if targeted update activation
    * probability of neighboring nodes
    */
  void live();

  void importIMMSeed();

  void runIMMLive();

  void initiateIMMProgressLog();

  void saveIMMProgress(int seen, int seedSize, double seedScore);

  // Save seed set to appropriate file
  void saveSeedSet(bool progress = false, int progPercentage = 0);

  // Import seed set at file_path starting a ../../data/<dataset>
  void importSeedSet(std::string file_path);

  // Function to test seed set at path ../../data/<dataset>/
  void seedSetTest(std::string file_path);

  /** Save live log data to file
    *
    * @param maxTime
    * @param runtime
    */
  void saveLiveLog(double& maxTime, double& runtime, std::string startDatetime, std::string endDatetime, double rtimScore, double immScore);

  /** Save live data to CSV file
    *
    * @param graph
    * @param streamTime
    * @param maxTime
    * @param runtime
    */
  void saveLiveCSV(const Graph& graph, double& streamTime, double& maxTime, double& runtime);

  /** Initiate progress log file with header
    * progress | seen | stream size | seed size
    */
  void initiateProgressLog();

  /** Save progress number of targeted seed nodes
    *
    * @param progress   : % of users seen in stream
    * @param seen       : number of users seen in stream
    * @param seedSize   : current size of seed set
    */
  void saveProgress(int seen, int seedSize, double seedScore);

  /** Initiate stream log file with head
    * stream_position | status | user_index | old_ap | ap | score | theta_score | ...
    */
  void initiateStreamLog();

  // Save stream log
  void saveStreamLog(int pos, int user, double ap, double ap_time, double oScore, double nScore, double inf_time, double theta_I, std::string rtim_status, int rtimSize, double rtimScore, double immScore, int inDeg, int outDeg);

  // Import influence scores
  void importScores();

  /** Print progress
    *
    * @param nb_threads
    * @param finishedProcess
    * @param numNodes
    * @param startTime
    * @param nb_nodes
    * @param save
    */
  int print_progress(int nb_threads, int finishedProcess, int numNodes, time_t startTime, int* nb_nodes, int save);

  /** Computes the influence score threshold index based on reach
    *
    * @param sorted: array of sorted influence scores
    */
  void getInfIndex(std::vector<double> & sorted);

  int run(std::string prevClass);

  // Menu to get stage arguments
  int stagesMenu(std::string prevClass);

  // Menu to input influence score pre-processing stage arguments
  int infScorePreProcessMenu(std::string prevClass);

  int apPreProcessMenu(std::string prevClass);

  // Menu to input live stage arguments
  int liveMenu(std::string prevClass);

  int liveIMMMenu(std::string prevClass);

  /** Menu to decide if user would like to continue program or quit
    * return -1 if quit
    */
  int continueMenu(std::string prevClass);

  std::string generateDataFilePath(std::string type);

  std::string generateFileName(std::string type, int param = 0);

  // Update activation probabilities of neighboring nodes
  void updateNeighborsAP(int src, std::set<int> path, double path_weight=1, int depth=2);

  /** Depth first recursive function to update neighbor's AP
    * We don't visit a neighbors who's AP = 1
    */
  void updateNeighborsAPDepth(int src, int maxDepth = 100);

  // update influence score of a node
  void updateInfluenceScore(double &infScore, int src, std::vector<double>& tmpAPs, int depth=2);
};
#endif

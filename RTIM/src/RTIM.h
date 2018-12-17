#ifndef __RTIM_H__
#define __RTIM_H__

#include <string>
#include <vector>

#include "Graph.h"
#include "Arguments.h"
#include "Tools.h"


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
  std::map<std::string, std::string> datasets = {
    { "test", "TS" },
    { "nethept", "NE" },
    { "dblp", "DB" },
    { "youtube", "YO" },
    { "livejournal", "LJ" },
    { "orkut", "OR" },
    { "twitter", "TW"}
  };
  std::map<std::string, std::string> keyword = {
    { "infScores", "infS"},
    { "seedSet", "ss"},
    { "stream", "st"},
    { "uniform_rand_repeat", "urr"},
    { "uniform_rand_no_repeat", "urnr"},
    { "inNOut_repeat", "inoutr"},
    { "progress", "prg"},
  };
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
  std::set<int> immSeedSet;
  std::string seedFile = "";
  std::vector<double> activationProbabilities; /**< array of activation probabilities*/
  Graph& graph;

  RTIM(Graph& g);

  /**
    *     PRE-PROCESSING
    *       FUNCTIONS
    *
    */

  /**
    * Pre-processing stage
    * Compute influence score of every user in graph
    *
    * @param graph: user influence network to use
    * @param max_depth: maximum depth for inf. score computation
    */
  void pre_process();//const Graph& graph);

  /**
    *
    */
  void initializeInfluenceScores();


  /**
    * Print influence scores
    */
  void printScores();

  /**
    * Saves influence scores to appropriate file
    */
  void saveScores();


  void printStageParams();

  /**
    *        LIVE
    *      FUNCTIONS
    *
    */

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
   * @param infReach
   *
   */
  void live();


  void importIMMSeed();


  /**
    * Save seed set to appropriate file
    */
  void saveSeedSet(bool progress = false, int progPercentage = 0);


  /**
    * Import seed set at file_path starting a ../../data/<dataset>
    */
  void importSeedSet(std::string file_path);


  /**
    * Function to test seed set at path ../../data/<dataset>/
    */
  void seedSetTest(std::string file_path);

  /**
    * Save live log data to file
    *
    * @param maxTime
    * @param runtime
    */
  void saveLiveLog(double& maxTime, double& runtime, std::string startDatetime, std::string endDatetime);


  /**
    * Save live data to CSV file
    *
    * @param graph
    * @param streamTime
    * @param maxTime
    * @param runtime
    */
  void saveLiveCSV(const Graph& graph, double& streamTime, double& maxTime, double& runtime);


  void saveSeedScoreLog(std::string file, std::string startDate, std::string endDate, double& runtime, double& score);


  void saveSeedScoreCSV(std::string file, std::string startDate, std::string endDate, double& runtime, double& score);

  /**
    * Initiate progress log file with header
    * progress | seen | stream size | seed size
    *
    */
  void initiateProgressLog();

  /**
    * Save progress number of targeted seed nodes
    *
    * @param progress   : % of users seen in stream
    * @param seen       : number of users seen in stream
    * @param seedSize   : current size of seed set
    */
  void saveProgress(int user_index, double ap, double score, int seen, double infTheta, int seedSize);


  /**
    * Initiate stream log file with head
    * stream_position | status | user_index | old_ap | ap | score | theta_score | ...
    *  
    */
  void initiateStreamLog();


  /**
    * Save stream log
    */
  void saveStreamLog(int pos, int user, double ap, double ap_time, double oScore, double nScore, double inf_time, double theta_I, std::string rtim_status, int seedSize, int imm_targeted, int inDeg, int outDeg);

  /**
    * Import influence scores
    */
  void importScores();


  /**
    * Save influence scores
    *
    * @param fileName
    */
  // void saveToCSV(std::string fileName);

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
    * Computes the influence score threshold index based on reach
    *
    * @param sorted: array of sorted influence scores
    */
  void getInfIndex(std::vector<double> & sorted);

  // /**
  //   * Compute number of outgoing neighbors and save to file
  //   */
  // void outgoing();
  //
  // /**
  //   * Merges outgoing file with influence score of defined file
  //   */
  // void mergeOutgoingScores();


  /**
    * Test time to compute score of a seed set
    */
  void seedComputationTest(int seedSize, int depth=10000, double minEdgeWeight=1.0);

  /**
    *    RTIM
    *    RUN
    */

  /**
    * Menu to get desired stage
    */
  void stageMenu();


  /**
    * Menu to get stage arguments
    */
  void stageArgumentsMenu();


  /**
    * Menu to input pre-processing stage arguments
    */
  void preProcessMenu();


  /**
    * Menu to input live stage arguments
    */
  void liveMenu();


  /**
    * Menu to input compute seed score arguments
    */
  void computeSeedScoreMenu();

  /**
    * Menu to decide if user would like to continue program or quit
    * return -1 if quit
    */
  int continueMenu();


  /**
    * Function to run program
    */
  void run();


  void runTest();


  void testStagesMenu();

  void testPreProcessScoresMenu();

  void testLiveMenu();

  void testContinueMenu();

  void rtimRun();

  /**
    *     MAIN
    *     CLASS
    */
  void mainMenu();


  // /**
  //   * Menu to get desired dataset
  //   * returns 0 if a new dataset needs to be imported else 1
  //   */
  // int datasetMenu();

  std::string generateDataFilePath(std::string type);

  std::string generateFileName(std::string type, int param = 0);
};
#endif

#ifndef __MAIN_H__
#define __MAIN_H__

// #include "Arguments.h"
#include "RTIM.h"
#include "Tools.h"
#include "Graph.h"

#include <string>

class Main{
public:
  // std::map<std::string, std::string> datasets = {
  //   { "test", "TS" },
  //   { "nethept", "NE" },
  //   { "dblp", "DB" },
  //   { "youtube", "YO" },
  //   { "livejournal", "LJ" },
  //   { "orkut", "OR" },
  //   { "twitter", "TW"}
  // };
  // std::map<std::string, std::string> keyword = {
  //   { "infScores", "infS"},
  //   { "seedSet", "ss"},
  //   { "stream", "st"},
  //   { "uniform_rand_repeat", "urr"},
  //   { "uniform_rand_no_repeat", "urnr"},
  //   { "inNOut_repeat", "inoutr"},
  //   { "progress", "prg"},
  // };
  std::string algorithm;            // [rtim, imm, compute_score, test]
  int datasetChoice;
  std::vector<std::string> datasetNames;
  std::vector<std::string> datasetIds;
  std::vector<int> datasetNodes;
  std::vector<int> datasetEdges;

  Graph graph;
  RTIM rtim;
  //IMM imm;

  Main();


  void loadDatasetsData();


  void run();


  void mainMenu();


  void datasetsMenu();


  void algorithmMenu();


  void experimentsMenu();


  void readExperiments(std::string path);


  void runExperiments(std::string path);


  void experimentsContinueMenu();


  void printDatasetArguments(int nodes, int edges);


  void getArguments(int argn, char **argv);


  void getArguments(std::string line);
};

#endif

#ifndef __MAIN_H__
#define __MAIN_H__

#include "Arguments.h"
#include "RTIM.h"
#include "Tools.h"
#include "Graph.h"

#include <string>

class Main{
public:
  std::string algorithm;            // [rtim, imm, compute_score, test]
  int datasetChoice;
  Arguments args;
  // Graph graph;
  // RTIM rtim;
  //IMM imm;

  Main();


  void mainMenu();


  void experimentsMenu();


  void experimentsContinueMenu();


  void datasetsMenu();


  void algorithmMenu();


  void runExperiments(std::string path);


  void run();
};

#endif

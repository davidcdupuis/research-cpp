#ifndef __MAIN_H__
#define __MAIN_H__


class Main{
public:
  std::string algorithm;            // [rtim, imm, compute_score, test]
  int datasetChoice;

  void mainMenu();


  void experimentsMenu();


  void experimentsContinueMenu();


  void datasetMenu();


  void algorithmMenu();


  void runExperiments(std::string path);


  void run();
};

#endif

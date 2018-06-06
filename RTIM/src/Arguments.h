#ifndef __ARGUMENTS_H__
#define __ARGUMENTS_H__

#include "Tools.h"
#include <string>

/* Class to read arguments from command line */
class Arguments{
public:
  int k;          // size of seed set if defined
  std::string dataset; //name of dataset
  std::string model;
  std::string stage;   // pre|live|evaluate
  int mc_depth;   // max depth for monte carlo simulations
  int mc_sim;     // number of simulations for monte carlo simulations

  /* Function to get arguments from command line */
  void getArguments(int argn, char **argv);

  /* Function to print arguments out to command line */
  void printArguments();
};
#endif

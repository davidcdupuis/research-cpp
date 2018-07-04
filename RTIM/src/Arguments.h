#ifndef __ARGUMENTS_H__
#define __ARGUMENTS_H__

#include "Tools.h"
#include <string>

/* Class to read arguments from command line */
class Arguments{
public:
  int k = -1;               // size of seed set if defined
  std::string dataset;      //name of dataset
  std::string availability; // name of availability model
  int version;              // version of stream
  std::string stage;        // pre|live|evaluate
  std::string model;        // model of weights: wc|ic
  int reach;                // reach to define inf. threshold = percentage
  double activation_threshold; // activation probability threshold [0, 1]
  int depth;                // max depth for monte carlo simulations

  /* Function to get arguments from command line */
  void getArguments(int argn, char **argv);

  /* Function to print arguments out to command line */
  void printArguments();

  /* Print help menu to command line*/
  void printHelp();
};
#endif

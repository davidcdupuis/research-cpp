#ifndef __ARGUMENTS_H__
#define __ARGUMENTS_H__

#include "Tools.h"
#include <string>

/* Class to read arguments from command line */
class Arguments{
public:
  int k = -1;                         // size of seed set if defined
  std::string dataset;                // name of dataset
  std::string streamModel;            // name of availability model
  int streamVersion;                  // version of stream
  int streamSize = -1;
  std::string stage;                  // pre|live|evaluate
  std::string model;                  // model of weights: wc|ic
  int reach = 20;                     // reach to define inf. threshold = percentage
  double activation_threshold = 0.8;  // activation probability threshold [0, 1]
  int depth;                          // max depth for monte carlo simulations

  /* Function to get arguments from command line */
  void getArguments(int argn, char **argv);

  /* Function to print arguments out to command line */
  void printArguments();

  /* Print help menu to command line*/
  void printHelp();
};
#endif

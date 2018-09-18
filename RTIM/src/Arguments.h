#ifndef __ARGUMENTS_H__
#define __ARGUMENTS_H__

#include "Tools.h"

#include <string>

/**
  * @file Arguments.h
  *
  * @author David Dupuis <david.dupuis@devinci.fr>
  * @version 3.0
  *
  * @section DESCRIPTION
  * Class to read arguments from command line
  *
  * Arguments are:
  *   - dataset
  *   - stage
  *   - ...
  */
class Arguments{
public:
  int k = 200;                          /**< size of seed set if defined */
  int streamVersion = 1;               /**< version of stream */
  int streamSize = -1;                 /**< size of stream to use*/
  int reach = 1;                       /**< reach to define inf. threshold = percentage */
  int depth = 2;                       /**< max depth for monte carlo simulations */

  std::string dataset;                /**< name of dataset */
  std::string streamModel = "rand_repeat"; /**< name of availability model */
  std::string stage;                  /**< pre|live|evaluate */
  std::string model = "ic";           /**< model of weights: ic|lt, default is ic */

  double theta_ap = 0.8;              /**< activation probability threshold [0, 1] */
  double edge_weight = -1;            /**< edge weight to use, default results in weighted cascade model */
  double min_weight = 0;              /**< minimum weight to continue influence propagation*/


  /**
    * Function to get arguments from command line
    *
    * @param argn
    * @param argv
    */
  void getArguments(int argn, char **argv);


  /**
    * Function to print arguments out to command line
    */
  void printArguments();


  /**
    * Print help menu to command line
    */
  void printHelp();
};
#endif

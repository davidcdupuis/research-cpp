#ifndef __ARGUMENTS_H__
#define __ARGUMENTS_H__

#include "Tools.h"

#include <string>
#include <sstream>
#include <iterator>
#include <iostream>

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

  int k = 200;                          /**< size of seed set if defined */
  int streamVersion = 1;               /**< version of stream */
  int streamSize = -1;                 /**< size of stream to use*/
  int depth = 2;                       /**< max depth for monte carlo simulations */

  std::string dataset;                /**< name of dataset */
  std::string streamModel = "inNOut_repeat"; /**< name of availability model */
  std::string stage;                  /**< pre|live|evaluate */
  std::string model = "ic";           /**< model of weights: ic|lt, default is ic */
  std::string seedSetPath;
  std::string algorithm;

  double reach = 1;                       /**< reach to define inf. threshold = percentage */
  double theta_ap = 0.8;              /**< activation probability threshold [0, 1] */
  double edge_weight = -1;            /**< edge weight to use, default results in weighted cascade model */
  double min_weight = 0;              /**< minimum weight to continue influence propagation*/


  std::vector<std::string> datasetNames;
  std::vector<std::string> datasetIds;
  std::vector<int> datasetNodes;
  std::vector<int> datasetEdges;


  /**
    * Function to get arguments from command line
    *
    * @param argn
    * @param argv
    */
  void getArguments(int argn, char **argv);


  void getArguments(std::string line);


  void loadDatasetsData();

  /**
    * Function to print arguments out to command line
    */
  void printArguments();


  void printDatasetArguments(int nodes, int edges);


  void printStageArguments();

  /**
    * Print help menu to command line
    */
  void printHelp();


  std::string generateDataFilePath(std::string type);

  /**
    Generate file_name from arguments
  */
  std::string generateFileName(std::string type, int param = 0);
};
#endif

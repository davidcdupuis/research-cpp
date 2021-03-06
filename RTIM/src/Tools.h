#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <iostream>
#include <set>
#include <list>
#include <sstream>
#include <cmath>
#include <queue>
#include <fstream>
#include <string>
#include <cstdio>
#include <functional>
#include <algorithm>
#include <climits>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <map>
#include <deque>
#include <dirent.h>
#include <unordered_set>
#include <random>

/**
  * Set of practical helper functions for other classes
  */

/**
  *
  */
  void printInColor(std::string color, std::string txt);


  std::string toColor(std::string color, std::string txt);
/**
  *
  */
  std::string cleanTime(double t, std::string type="ms");


/**
  *
  */
  void clearLines(int l);


/**
  *
  */
  void printLocalTime(std::string color, std::string name, std::string status);


  std::string getLocalDatetime();

/**
  *
  */
  std::string printSeed(std::vector<int> seed);


  std::string properStringDouble(double value);


  std::string cleanLongInt(int value);

  void listFolderFiles(std::string folder);

  // function to check existence of folder
  bool pathExists(const std::string& filename);

  std::vector<int> randomSeedGenerator(int N, int k);

  std::unordered_set<int> setGenerator(int N, int k, std::mt19937& gen);

  void printTitle(int size, std::string text, std::string color="red", char delim='-');

#endif

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

/**
  * Set of practical helper functions for other classes
  */

/**
  *
  */
  void printInColor(std::string color, std::string txt);


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

/**
  *
  */
  std::string printSeed(std::vector<int> seed);
#endif

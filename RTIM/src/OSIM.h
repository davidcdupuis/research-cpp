#ifndef __OSIM_H__
#define __OSIM_H__

#include <iostream>
#include <string>
#include <unistd.h>
#include <set>
#include <algorithm>

#include "Graph.h"
#include "Tools.h"

/* Optimal Size Influence Maximization (OSIM)
 * An IM algorithm which can do three things:
 * 1. Based on a specified rule it can reduce a graph to a certain % of it's
 *    original size in number of nodes
 * 2. For a given graph it can compute the optimal size of the seed set which
 *    solves the influence maximization problem
 * 3. Based on the input graph it can compute the nodes inside a seed set with
 *    an optimal size
 */
class OSIM{
public:
  int optimalSize;
  std::vector<std::vector<std::pair<int, double> > > osimGraph; //necessary
  Graph& graph;

  OSIM(Graph& g);

  /* reduces graph to a certain % of it's original size
   * It does this by removing nodes in priority with few outgoing edges, i.e.
   * that don't influence lots of other nodes
   */
  void reduceGraph(int top);

  int selectRandomUser(set<int>& s);

  /* Finds the optimal size given a graph
   * Graph can be full size or a reduction
   * @params sim = number of simulations we run
   */
  void findOptimalSize(int sim = 10);

  /** run the same algoritm as for optimal size
    * save seed sets and randomly pick one with the same optimal size (how many?)
    * test influence score
    */
  void findRandomSeedSet();

  /** run the same algorithm as for optimal size
    * save seed sets, for all sets with optimal size compute the influence score
    * choose best
    */
  void findBestSeedSet();

  // uses frequency of nodes in seed sets to build best seed set
  void findFrequencySeedSet();

  int run(std::string prevClass);

  int functionsMenu(std::string prevClass);

  int OSIM::optSizeMenu(std::string prevClass);

  int OSIM::seedSizeMenu(std::string prevClass);

  int continueMenu(std::string prevClass);
}

#endif

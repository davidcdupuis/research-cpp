#ifndef __OSIM_H__
#define __OSIM_H__

#include <iostream>
#include <string>
#include <unistd.h>

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
  void reduceGraph(int size);

  /* Finds the optimal size given a graph
   * Graph can be full size or a reduction
   */
  void findOptimalSize();

  /* function to find the best seed set for the given graph considering the
   * optimal seed set size
   * It does this by:
   * 1. First picking uniformily at random a user, simulating spread, and
   *    repeating until all users in the graph have been activated. It repeats
   *    this steps as many times as possible creating sample seed sets.
   * 2. Then it ... ?
   */
  void seedSet();
}

#endif

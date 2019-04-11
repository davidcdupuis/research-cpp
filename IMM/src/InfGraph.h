#ifndef __INFGRAPH_H__
#define __INFGRAPH_H__

#include <vector>
#include <utility>
#include <string>
#include <set>
#include <deque>

#include "iheap.h"
#include "Graph.h"
#include "Arguments.h"
#include "Counter.h"

typedef pair<double, int> dipair;

class InfGraph : public Graph
{
private:
	std::vector<bool> visit;
	std::vector<int> visit_mark;

public:
	std::vector< std::vector<int>> hyperG;
	std::vector< std::vector<int>> hyperGT;
	deque<int> q;
	sfmt_t sfmtSeed;
	set<int> seedSet;

  	InfGraph(std::string folder, std::string graph_file);
  	void init_hyper_graph();
  	void build_hyper_graph_r(int64 R, const Arguments & arg);
  	void build_seedset(int k);
  	double InfluenceHyperGraph();
  	int BuildHypergraphNode(int uStart, int hyperiiid);
};

#endif

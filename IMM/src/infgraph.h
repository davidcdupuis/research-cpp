#ifndef __INFGRAPH_H__
#define __INFGRAPH_H__

#include "iheap.h"
#include "Arguments.h"

typedef pair<double,int> dipair;

class InfGraph:public Graph{
private:
  vector<bool> visit;
  vector<int> visit_mark;

public:
  vector<vector<int>> hyperG;
  vector<vector<int>> hyperGT;
  InfGraph(std::string folder, std::string graph_file);
  void init_hyper_graph();
  void build_hyper_graph_r(int64 R, const Arguments & arg);
  void build_seedset(int k);
  double InfluenceHyperGraph();
  int BuildHypergraphNode(int uStart, int hyperiiid);
};

#endif

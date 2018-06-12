#include "RTIM.h"
#include "Arguments.h"
#include "Graph.h"
#include <vector>

using namespace std;

void RTIM::pre_process(const Graph& graph){
  // for each node in graph compute influence score
  cout << "Running pre_process on " << dataset << endl;
  double score = 0;
  for(int node: graph){
    score = graph.influenceScore(node);
    infScores[node] = score;
    // cout << node << " : " << infScore[node] << endl;
  }
  cout << "Pre_process complete!" << endl;
};

void RTIM::live(const Graph& graph){
  cout << "Running live on " << dataset << endl;
  // run live
  cout << "Live complete!" << endl;
};

void RTIM::printScores(){
  cout << "Influence Scores: " << endl;
  for (int node: infScores){
    cout << "(" << node << " : " << infScores[node] << ")" << endl;
  }
}

int main(int argn, char **argv)
{
    Arguments args = Arguments();
    args.getArguments(argn, argv);
    args.printArguments();
    Graph g = Graph(args.dataset);
    //g.print();
/*
    vector<int> v;
    v.push_back(0);
    double s = g.influenceScore(v);
*/
}

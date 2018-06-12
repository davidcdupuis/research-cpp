#include "RTIM.h"
#include "Arguments.h"
#include "Graph.h"
#include <vector>

using namespace std;


RTIM::RTIM(string d){
  dataset = d;
}

void RTIM::pre_process(const Graph& graph){
  // for each node in graph compute influence score
  cout << "Running pre_process on " << dataset << endl;
  infScores.reserve(graph.graph.size());
  double score;
  for(int i = 0; i < graph.graph.size(); i++){
    score = graph.influenceScore({i}, 3); 
    infScores[i] = score;
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
    
    RTIM rtim = RTIM(args.dataset);

    rtim.pre_process(g);
    //g.print();
/*
    vector<int> v;
    v.push_back(0);
    double s = g.influenceScore(v);
*/
}

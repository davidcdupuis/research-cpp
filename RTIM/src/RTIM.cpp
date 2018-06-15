#include <vector>
#include <cstdio>
#include <ctime>
#include <omp.h>

#include "RTIM.h"
#include "Arguments.h"
#include "Graph.h"

using namespace std;


RTIM::RTIM(string d){
  dataset = d;
}

void RTIM::pre_process(const Graph& graph){
  // for each node in graph compute influence score
  cout << "Running pre_process on " << dataset << endl;
  double score;
  numNodes = graph.graph.size();

  infScores.reserve(numNodes);
  for(int i = 0; i < numNodes; i++){
    infScores.push_back(0);
  }

  #pragma omp parallel for private(score) shared(infScores, graph)
  for(int i = 0; i < numNodes; i++){
    score = graph.influenceScore({i}, 3);
    infScores[i] = score;
  }
  if (numNodes <= 20){
    printScores();
  }
  saveScores();
  cout << "Pre_process complete!" << endl;
};

void RTIM::live(const Graph& graph){
  cout << "Running live on " << dataset << endl;
  // run live
  cout << "Live complete!" << endl;
};

void RTIM::printScores(){
  cout << "Influence Scores: " << endl;
  for (int i = 0; i < numNodes ; i++){
    cout << "(" << i << " : " << infScores[i] << ")" << endl;
  }
}

void RTIM::saveScores(){
  string file = "../../data/algorithms/rtim/" + dataset + "_infscores.txt";
  cout << "Saving influence scores to: " << file << endl;
  // save scores
  ofstream infScoresFile;
  infScoresFile.open(file);
  for (int i = 0; i < numNodes ; i++){
     infScoresFile << i << " " << infScores[i] << endl;
  }
  infScoresFile.close();
  cout << "Scores saved successfully!" << endl;
}

void RTIM::importScores(){
  cout << "Importing influence scores" << endl;
  // import scores
  cout << "Import successful" << endl;
}

int main(int argn, char **argv)
{
    clock_t start;
    double duration;

    Arguments args = Arguments();
    args.getArguments(argn, argv);
    args.printArguments();
    start = clock();
    Graph g = Graph(args.dataset);
    duration = (clock() - start)/(double)CLOCKS_PER_SEC;
    cout << "Import done in: " << duration << " seconds." << endl;
    RTIM rtim = RTIM(args.dataset);

    //start = clock();
    rtim.pre_process(g);
    //duration = (clock() - start)/(double)CLOCKS_PER_SEC; // duration in seconds
    // cout << "Pre_process ran in: " << duration << " seconds." << endl;
    //g.print();
/*
    vector<int> v;
    v.push_back(0);
    double s = g.influenceScore(v);
*/
}

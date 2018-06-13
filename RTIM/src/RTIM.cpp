#include "RTIM.h"
#include "Arguments.h"
#include "Graph.h"
#include <vector>
#include <cstdio>
#include <ctime>
#include <omp.h>
#include <random>

using namespace std;


RTIM::RTIM(string d){
  dataset = d;
}

void RTIM::pre_process(const Graph& graph){
  // for each node in graph compute influence score
  cout << "Running pre_process on " << dataset << endl;
  infScores.reserve(graph.graph.size());
  double score;
  int nodes = graph.graph.size();
  
  #pragma omp parallel for private(score) shared(infScores, graph)
  for(int i = 0; i < nodes; i++){
    random_device rd;
    //unsigned seed = time(NULL) + rd()*(i+1)*omp_get_thread_num();
    //cout << "Thread num: " << omp_get_thread_num() << ", seed: " << seed << endl;
    //cout << "Thread number: " << omp_get_thread_num() << " : " << i << endl;
    score = graph.influenceScore({i}, rd(), 3);
    cout << "<" << score << "> "; 
    infScores[i] = score;
  }
  cout << "scores: " << infScores.size() << endl;
  if (nodes <= 20){
    printScores();
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
    clock_t start;
    double duration;

    Arguments args = Arguments();
    args.getArguments(argn, argv);
    args.printArguments();
    
    Graph g = Graph(args.dataset);
    
    RTIM rtim = RTIM(args.dataset);

    start = clock();
    rtim.pre_process(g);
    duration = (clock() - start)/(double)CLOCKS_PER_SEC; // duration in seconds
    cout << "Pre_process ran in: " << duration << " seconds." << endl;
    //g.print();
/*
    vector<int> v;
    v.push_back(0);
    double s = g.influenceScore(v);
*/
}

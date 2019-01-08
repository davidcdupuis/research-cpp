#include "IMM.h"

IMM::IMM(Graph& g):graph(g){

}

IMM::run(){
  sampling();
  nodeSelection();
  // print seed set
}

double IMM::sampling(){
  double epsilon_prime = epsilon * sqrt(2);
  int x = 1;
  while(true){
    // do stuff
    return 1.0;
    x++;
  }
}

double IMM::nodeSelection(){
  double alpha = sqrt(log(graph.nodes) + log(2));
  double beta = 0;
  // double beta = sqrt((1-1/e) * ) ...
  int64 R = 2.0 * graph.nodes * pow((1 - 1/e) * alpha + beta, 2);

  // build hypergraph
  // build seed set
  // compute InfluenceHyperGraph
  return 1.0;
}

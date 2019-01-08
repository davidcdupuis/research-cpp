#include "OSIM.h"

OSIM::OSIM(Grap& g):graph(g){

}

void OSIM::reduceGraph(int top){
  // rank nodes based on outDegrees, remove 100 - top % = bottom %
  // re-generate graph without those nodes
}

void OSIM::findOptimalSize(){
  // 1. randomly choose a node from the body of non-activated users
  // 2. simulate influence from that node, recording activated users
  // 3. repeat 1. and 2. until all nodes have been selected
  // repeat 1. 2. and 3. n number of times
}

void OSIM::findRandomSeedSet(){
  //
}

void OSIM::findBestSeedSet(){
  //
}

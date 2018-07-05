#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <omp.h>
#include <random>
#include <map>
#include <set>

#include "Graph.h"

using namespace std;

string printSeed(vector<int> seed){
  string s = "[";
  for (int i = 0; i < seed.size(); i++){
    if (i < seed.size() - 1){
      s += to_string(seed[i]) + ", ";
    }else{
      s += to_string(seed[i]);
    }
  }
  s += "]";
  return s;
}

Graph::Graph(string d, bool import){
  //srand(time(NULL));
  dataset =  d;
  readAttributes();
  if (import){
    graph.resize(nodes);
    loadGraph();
  }
}

/* Function to add an edge */
void Graph::addEdge(int a, int b, double w){
  graph[a].push_back(make_pair(b, w));
}

/* Function to read attributes.txt */
void Graph::readAttributes(){
  string folder = "../../data/" + dataset + "/attributes.txt";
  ifstream infile(folder.c_str());
  cout << "Loading attributes from: " << folder << endl;
  string s;
  while(infile >> s){
    if(s.substr(0, 2)=="n="){
      nodes = atoi(s.substr(2).c_str());
      cout << "Number of nodes: " << nodes << endl;
      continue;
    }
    if (s.substr(0, 2) == "m="){
      edges = atoi(s.substr(2).c_str());
      cout << "Number of edges: " << edges << endl;
      continue;
    }
    cerr << "Error: bad attributes!" << endl;
    exit(1);
  }
  cout << "---------------------------------------" << endl;
  infile.close();
}

/* Function to import graph from file */
void Graph::loadGraph(){
  string graph_file = "../../data/" + dataset + "/" + dataset + "_wc.inf";
  FILE *fin = fopen(graph_file.c_str(), "r");
  if (!(fin != 0)){
    cerr << "ASSERT FAIL @ " << __FILE__ << ":" << __LINE__ << endl;
    exit(1);
  }
  cout << "Loading graph from: " << graph_file << endl;
  int readCnt = 0;
  for (int i = 0; i < edges; i++)
  {
      readCnt ++;
      int user1, user2;
      double weight;
      int c = fscanf(fin, "%d%d%lf", &user1, &user2, &weight);
      if (c != 3 && c!= -1) {
        cerr << "ASSERT FAIL @ "<< __FILE__ << ":" << __LINE__ << endl;
        cerr << "Info: " << user1 << ", " << user2 << ", " << weight << ", "
             << c << endl;
        exit(1);
      }
      // check if node ids within graph array range
      if(user1 >= nodes){
        cerr << "ASSERT FAIL @ " << __FILE__ << ":" << __LINE__ << ":("
             << user1 << " >= " << nodes << ")" << endl;
        exit(1);
      }
      if(user2 >= nodes){
        cerr << "ASSERT FAIL @ " << __FILE__ << ":" << __LINE__ << ":("
             << user2 << " >= " << nodes << ")" << endl;
        exit(1);
      }
      //cout << "(" << user1 << ")" << "-[" << weight << "]->" << "(" << user2 << ")" << endl;
      addEdge(user1, user2, weight);
  }
  // TRACE_LINE_END();
  // int s = 0;
  // for (int i = 0; i < nodes; i++)
  //     if (hasnode[i])
  //         s++;
  // INFO(s);
  // ASSERT(readCnt == m);
  cout << "Graph loaded successfully!" << endl;
  fclose(fin);
}

/* Function to calculate influence score of seed set */
double Graph::influenceScore(const vector<int>& seed_set, int depth, int sim) const{
  // cout << "Computing influence score of: " << printSeed(seed_set) << endl;
  int sum = 0;
  for (int i = 0; i < sim; i++){
    // run influence simulation
    sum += influenceSimulation(seed_set, depth);
  }
  double score = sum/(double)sim;
  // cout << "Influence score is " << score << endl;
  return score;
}

double Graph::influenceScorePath(int node, int max_depth, string type) const{
  // if type == 'shortest' use shortest paths, if 'all' use all paths, else return error
  double score = 1;
  if (type == "shortest"){
    // use function of shortest paths of length at most max_depth
    // if node doesn't have neighbor return 1
    if(graph[node].size() == 0){
      score = 1;
    }else{
      map< int, double > minDistances;
      shortestPathsWeights(minDistances, node, max_depth);
      map< int, double >::iterator it = minDistances.begin();
      while(it != minDistances.end()){
        score += minDistances[it->first];
        it++;
      }
      /*for (pair<int, double> dist: minDistances){
        score += dist.second;
      }*/

      //double* minDistances = calloc(sizeof(double)*graph.size());
      //shortestPathsWeights2(minDistances, node, max_depth);


    }
  }else if (type == "all"){
    score = 1;
  }else{
    cerr << "Error path type: " << type << " does not exist!" << endl;
    exit(1);
  }
  return score;
}

double Graph::influenceScoreNeighbors(int node) const{
  double score = 1;
  int lim = graph[node].size();
  for(int i=0; i < lim; i++){
    score += graph[node][i].second;
  }
  return score;
}

/* Function to perform influence coverage from seed set */
int Graph::influenceSimulation(const vector<int>& seed_set, int depth) const{
  int activated = 0;
  vector<int> activated_nodes;
  // seed nodes are already activated
  for(int node: seed_set){
    activated_nodes.push_back(node);
  }
  // simulate inf propagation from each seed node
  random_device rd;
  unsigned seed = rd();
  for (int node: seed_set){
    queue< pair<int, int> > queue;
    activated += 1;
    queue.push(make_pair(node, 0));
    while(!queue.empty()){
      pair<int, int> curr = queue.front();
      queue.pop();
      // cout << "(" << curr.first << ", " << curr.second << ") ";
      // test influence of all neigbhors
      for(pair<int, double> neighbor: graph[curr.first]){
        // check if neighbor is not in activated nodes
        double r = rand_r(&seed)/(double)RAND_MAX;
        if (!(find(activated_nodes.begin(), activated_nodes.end(), neighbor.first)!=activated_nodes.end())
            && (r <= neighbor.second)){

          // if influence increment activated,
          // add to queue, and activated_nodes and increase depth.
          activated += 1;
          activated_nodes.push_back(neighbor.first);
          if(curr.second + 1 <= depth){
            queue.push(make_pair(neighbor.first, curr.second + 1));
          }
        }
      }
    }
  }
  //cout << "(" << seed_set[0] << "," << activated << ") ";
  return activated;
}

void Graph::shortestPathsWeights(map<int, double> & distances, int node, int max_depth, double curr_dist) const{
  if (max_depth == 0) {
    return;
  }

  for(pair<int, double> neighbor: graph[node]){
    double new_dist = curr_dist * neighbor.second;
    // check if new_path distance is greater than older one.
    if(distances.find(neighbor.first) == distances.end()){
      distances[neighbor.first] = 0;
    }
    distances[neighbor.first] = max(distances[neighbor.first], new_dist);
    shortestPathsWeights(distances, neighbor.first, max_depth - 1, new_dist);
  }
}

void Graph::shortestPathsWeightsB(double* distances, int node, int max_depth, double curr_dist) const{
  if (max_depth == 0) {
    return;
  }

  for(pair<int, double> neighbor: graph[node]){
    double new_dist = curr_dist * neighbor.second;
    // check if new_path distance is greater than older one.
    distances[neighbor.first] = max(distances[neighbor.first], new_dist);
    shortestPathsWeightsB(distances, neighbor.first, max_depth - 1, new_dist);
  }
}

void Graph::updateNeighborsAP(int src, vector<double>& activationProbs, set<int> path, double path_weight, int depth){
  path.insert(src);
  double new_path_weight;
  for(pair<int, double> neighbor: graph[src]){
    if(path.find(neighbor.first) != path.end()){
      new_path_weight = path_weight * neighbor.second;
      activationProbs[neighbor.first] = 1 - (1 - activationProbs[neighbor.first])*(1 - new_path_weight);
      if (depth > 1){
        updateNeighborsAP(neighbor.first, activationProbs, path, new_path_weight, depth - 1);
      }
    }
  }
  path.erase(src);
}

void Graph::print(){
  cout << dataset << " graph:" << endl;
  for(int i = 0; i < nodes; i++){
    for (int j = 0; j < graph[i].size(); j++){
	cout << "(" << i << ")" << "-[" << graph[i][j].second << "]->"
           << "(" << graph[i][j].first << ")" << endl;
    }
  }
}

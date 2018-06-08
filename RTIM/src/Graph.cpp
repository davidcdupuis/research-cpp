#include <iostream>
#include <fstream>
#include "Graph.h"

using namespace std;


Graph::Graph(string d){
  dataset =  d;
  readAttributes();
  graph.resize(nodes);
  //loadGraph();
}

/* Function to add an edge */
void Graph::addEdge(int a, int b, double w){
  graph[a].push_back(make_pair(b, w));
}

/* Function to read attributes.txt */
void Graph::readAttributes(){
  string folder = "../../data/" + dataset + "/attribute.txt";
  //ifstream infile;
  //infile.open(folder.c_str());
  //ifstream cin(folder.c_str());
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
  string graph_file = "../data/" + dataset + "/graph_ic.inf";
  FILE *fin = fopen(graph_file.c_str(), "r");
  if (!(fin != false)){
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
      if (c != 3) {
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
      addEdge(user1, user2, weight);
  }
  // TRACE_LINE_END();
  // int s = 0;
  // for (int i = 0; i < nodes; i++)
  //     if (hasnode[i])
  //         s++;
  // INFO(s);
  // ASSERT(readCnt == m);
  fclose(fin);
}

/* Function to calculate influence score of seed set */
double Graph::influenceScore(int sim){
  double score;
  for (int i = 0; i < sim; i++){
    // run random walk
  }
}

/* Function to perform influence coverage from seed set */
int Graph::influenceCoverage(){
  int score = 0;
  return score;
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

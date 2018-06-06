#include "Graph.h"
#include "Tools.h"

using namespace std;


Graph::Graph(string d){
  dataset =  "../data/" + dataset;
  readAttributes();
  graph.resize(nodes);
  loadGraph();
}

/* Function to add an edge */
void Graph::addEdge(int a, int b, double w){
  graph[a].push_back(make_pair(b, w));
}

/* Function to read attributes.txt */
void Graph::readAttributes(){
  string folder = "../data/" + this->dataset + "/attribute.txt";
  ifstream cin(folder.c_str());
  // ASSERT(!cin == false);
  if(!(!cin == false)){
    cerr << "ASSERT FAIL @ "<< __FILE__ << ":" << __LINE__ <<endl;
    exit(1);
  }
  string s;
  while (cin >> s)
  {
      if (s.substr(0, 2) == "n=")
      {
          nodes = atoi(s.substr(2).c_str());
          continue;
      }
      if (s.substr(0, 2) == "m=")
      {
          edges = atoi(s.substr(2).c_str());
          continue;
      }
      exit(1);
  }
  TRACE(nodes, edges);
  cin.close();
}

/* Function to import graph from file */
void Graph::loadGraph(){
  string graph_file = "../data/" + this->dataset + "/graph_ic.inf";
  FILE *fin = fopen(graph_file.c_str(), "r");
  ASSERT(fin != false);
  if (fin == true){
    cerr << "ASSERT FAIL @ " << __FILE__ << ":" << __LINE__ << endl;
    exit(1);
  }
  int readCnt = 0;
  for (int i = 0; i < edges; i++)
  {
      readCnt ++;
      int user1, user2;
      double weight;
      int c = fscanf(fin, "%d%d%lf", &user1, &user2, &weight);
      ASSERTT(c == 3, user1, user2, weight, c);

      //TRACE_LINE(a, b);
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
      // hasnode[user1] = true;
      // hasnode[user2] = true;
      this->addEdge(user1, user2, weight);
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

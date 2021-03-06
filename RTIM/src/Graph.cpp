#include "Graph.h"

using namespace std;

const int SLEEP = 2;

Graph::Graph(){
  // cout << "Graph created!" << endl;
  // cout << "loaded: " << loaded << endl;
}

void Graph::addEdge(int a, int b, double w){
  graph[a].push_back(make_pair(b, w));
}

void Graph::readAttributes(string folder){
  if (folder == ""){
    folder = "../../data/" + dataset + "/attributes.txt";
  }
  // cout << "Attributes file: " << folder << endl;
  ifstream infile(folder.c_str());
  //cout << "Loading attributes from: " << folder << endl;
  string s;
  while(infile >> s){
    if(s.substr(0, 2)=="n="){
      nodes = atoi(s.substr(2).c_str());
      // cout << "Number of nodes: " << nodes << endl;
      continue;
    }
    if (s.substr(0, 2) == "m="){
      edges = atoi(s.substr(2).c_str());
      // cout << "Number of edges: " << edges << endl;
      continue;
    }
    cerr << "Error: bad attributes in " << __FILE__ << " at line: " << __LINE__ << endl;
    exit(1);
  }
  infile.close();
}

void Graph::loadGraph(string graph_file){
  graph = {};
  graph.resize(nodes);
  // cout << "Graph size:" << graph.size() << endl;
  clock_t start = clock();
  if (graph_file == ""){
    graph_file = "../../data/" + dataset + "/" + dataset + "_wc.inf";
  }
  // cout << "Graph file: " << graph_file << endl;
  if (!pathExists(graph_file)){
    cerr << "Error path doesn't exist: " << graph_file << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  // cout << "Loading graph from: " << graph_file << endl;
  FILE *fin = fopen(graph_file.c_str(), "r");
  if (!(fin != 0)){
    cerr << "ASSERT FAIL @ " << __FILE__ << ":" << __LINE__ << endl;
    exit(1);
  }
  int barWidth = 50;
  for (int i = 0; i < edges; i++)
  {
    int user1, user2;
    double weight;
    int c = fscanf(fin, "%d%d%lf", &user1, &user2, &weight);
    if (c != 3 && c!= -1) {
      cerr << "ASSERT FAIL @ "<< __FILE__ << ":" << __LINE__ << endl;
      cerr << "Info: " << user1 << ", " << user2 << ", " << weight << ", " << c << endl;
      exit(1);
    }
    // check if node ids within graph array range
    if(user1 >= nodes){
      cerr << "ASSERT FAIL @ " << __FILE__ << ":" << __LINE__ << ":(" << user1 << " >= " << nodes << ")" << endl;
      exit(1);
    }
    if(user2 >= nodes){
      cerr << "ASSERT FAIL @ " << __FILE__ << ":" << __LINE__ << ":(" << user2 << " >= " << nodes << ")" << endl;
      exit(1);
    }
    // if (edgeWeight == -1){
    //   addEdge(user1, user2, weight);
    // } else { // if(args.edge_weight!= -1 && (args.edge_weight >= 0.0 || args.edge_weight <= 1.0))
    //   addEdge(user1, user2, args.edge_weight);
    // }
    addEdge(user1, user2, weight);
    // print progress
    /*
    float progress = (float)i/edges;
    cout << "\r[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) {
          cout << "=";
        } else if (i == pos){
          cout << ">";
        } else {
          cout << " ";
        }
    }
    cout << "] " << int(progress * 100.0) << " % -- (imported edges: " << i << ")";
    cout.flush();
    */
  }
  double duration = (clock() - start)/(double)CLOCKS_PER_SEC;
  // cout << "Graph import done in: " << cleanTime(duration, "s") << endl;
  fclose(fin);
  // sleep(SLEEP);
  // clearLines(2);
}

void Graph::importDegrees(){
  string path = "../../data/" + dataset + "/" + dataset + "_degrees.txt";
  int user, inDeg, outDeg;
  // if (!pathExists(path)){
  //   cerr < "Error in importDegrees, path doesn't exist: " << path << endl;
  //   exit(1);
  // }
  // printInColor("cyan", "User degrees : " + dataset + "_degrees.txt");
  cout << "User degrees : " << toColor("cyan", dataset + "_degrees.txt") << endl;
  inDegrees.resize(nodes, 0);
  outDegrees.resize(nodes, 0);
  ifstream infile(path.c_str());
  string str;
  getline(infile, str);
  while(infile >> user >> inDeg >> outDeg){
    inDegrees[user] = inDeg;
    outDegrees[user] = outDeg;
  }
  // printInColor("cyan", "Import degrees successful");
}

void Graph::printArguments(){
  // cout << string(26, '-') << toColor("red", " Graph ") << string(25, '-') << endl;
  printTitle(60, "Graph");
  cout << "\t" << toColor("yellow", datasetFile) << endl;
  cout << "\t"<< toColor("yellow", dataset);
  cout << " [ v = " << toColor("yellow", to_string(nodes)) << " | ";
  cout << "e = " << toColor("yellow", to_string(edges))  << "]" << endl;
  cout << "\t"<< toColor("yellow", infModel) << " | ";
  if(edgeWeight == -1){
    cout << toColor("yellow", "WC") << " | ";
  }else{
    cout << toColor("yellow", properStringDouble(edgeWeight)) << " | ";
  }

  cout << ((loaded) ? (toColor("green", "loaded")) : (toColor("red", "loaded"))) << endl;
  cout << string(60, '-') << endl;
}

void Graph::print(){
  cout << dataset << " graph:" << endl;
  for(int i = 0; i < nodes; i++){
    for (int j = 0; j < graph[i].size(); j++){
      cout << "(" << i << ")" << "-[" << graph[i][j].second << "]->"<< "(" << graph[i][j].first << ")" << endl;
    }
  }
}

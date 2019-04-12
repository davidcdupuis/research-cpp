#include "Graph.h"

using namespace std;

Graph::Graph(const Arguments & args){
  dataset = args.dataset;
  subgraph = args.subgraph;
  subsize = args.subsize;
  submodel = args.submodel;

  if (subgraph){
    folder = "../../data/" + dataset + "/osim/";// + dataset + "_" + to_string(subsize) + "_wc.inf";
  }else{
    folder = "../../data/" + dataset + "/";// + dataset +"_wc.inf";
  }

  readAttributes();
  //init vector
  for(int i = 0; i < nodes; i++){
    gT.push_back(vector<int>());
    hasnode.push_back(false);
    probT.push_back(vector<double>());
    inDeg.push_back(0);
  }
  readGraph();
}

void Graph::setInfluModel(InfluModel p){
  influModel = p;
}

void Graph::readAttributes(){
  string file_path = "";
  if(!subgraph){
    file_path = folder + "attributes.txt";
  }else{
    file_path = folder + "attributes_" + to_string(subsize) + ".txt";
  }
  cout << file_path << endl;
  ifstream cin(file_path.c_str());
  ASSERT(!cin == false);
  string s;
  while (cin >> s){
    if (s.substr(0, 2) == "n="){
      nodes = atoi(s.substr(2).c_str());
      continue;
    }
    if (s.substr(0, 2) == "m="){
      edges = atoi(s.substr(2).c_str());
      continue;
    }
    ASSERT(false);
  }
  //TRACE(nodes, edges );
  cin.close();
  printInfo();
}

void Graph::addEdge(int a, int b, double p){
  probT[b].push_back(p);
  gT[b].push_back(a);
  inDeg[b]++;
}

void Graph::readGraph(){
  string file_path = "";
  if(!subgraph){
    file_path = folder + dataset +"_wc.inf";
  }else{
    file_path = folder + dataset + "_wc_" + to_string(subsize) + ".inf";
  }
  cout << file_path << endl;
  FILE *fin = fopen(file_path.c_str(), "r");
  ASSERT(fin != false);
  int readCnt = 0;
  for (int i = 0; i < edges; i++){
    readCnt ++;
    int a, b;
    double p;
    int c = fscanf(fin, "%d%d%lf", &a, &b, &p);
    ASSERTT(c == 3, a, b, p, c);

    //TRACE_LINE(a, b);
    // check if node ids within graph array range
    ASSERT( a < nodes );
    ASSERT( b < nodes );
    hasnode[a] = true;
    hasnode[b] = true;
    addEdge(a, b, p);
  }
  //TRACE_LINE_END();
  int s = 0;
  for (int i = 0; i < nodes; i++)
      if (hasnode[i])
        s++;
  INFO(s);
  ASSERT(readCnt == edges);
  fclose(fin);
}

void Graph::readGraphBin(){
    string graph_file_bin = folder.substr(0, folder.size() - 3) + "bin";
    INFO(graph_file_bin);
    FILE *fin = fopen(graph_file_bin.c_str(), "rb");
    //fread(fin);
    struct stat filestatus;
    stat( graph_file_bin.c_str(), &filestatus );
    int64 sz = filestatus.st_size;
    char *buf = new char[sz];
    int64 sz2 = fread(buf, 1, sz, fin);
    INFO("fread finish", sz, sz2);
    ASSERT(sz == sz2);
    for (int64 i = 0; i < sz / 12; i++){
      int a = ((int *)buf)[i * 3 + 0];
      int b = ((int *)buf)[i * 3 + 1];
      float p = ((float *)buf)[i * 3 + 2];
      //INFO(a,b,p);
      addEdge(a, b, p);
    }
    delete []buf;
    fclose(fin);
}

void Graph::printInfo(){
  cout << string(20, '-') << toColor("red", " Graph ") << string(23, '-') << endl;
  cout << "- dataset    : " << toColor("yellow", dataset) << endl;
  cout << "- nodes      : " << toColor("yellow", to_string(nodes)) << endl;
  cout << "- edges      : " << toColor("yellow", to_string(edges)) << endl;
  cout << string(50, '-') << endl;
}

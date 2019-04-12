#include "Graph.h"

using namespace std;

Graph::Graph(string name, string folder, string graph_file): folder(folder), graph_file(graph_file){
  dataset = name;
  readAttributes();
  //init vector
  for(int i = 0; i < n; i++){
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
  // if(arg.subgraph){
  //   cout << ("../data/" + folder + "/attributes_" + arg.subsize).c_str() << endl;
  //   ifstream cin (("../data/" + folder + "/attributes_" + arg.subsize).c_str());
  // }
  cout << ("../../data/" + folder + "/attributes.txt").c_str() << endl;
  ifstream cin(("../../data/" + folder + "/attributes.txt").c_str());
  ASSERT(!cin == false);
  string s;
  while (cin >> s){
    if (s.substr(0, 2) == "n="){
      n = atoi(s.substr(2).c_str());
      continue;
    }
    if (s.substr(0, 2) == "m="){
      m = atoi(s.substr(2).c_str());
      continue;
    }
    ASSERT(false);
  }
  //TRACE(n, m );
  cin.close();
  printInfo();
}

void Graph::addEdge(int a, int b, double p){
  probT[b].push_back(p);
  gT[b].push_back(a);
  inDeg[b]++;
}

void Graph::readGraph(){
  cout << graph_file.c_str() << endl;
  FILE *fin = fopen((graph_file).c_str(), "r");
  ASSERT(fin != false);
  int readCnt = 0;
  for (int i = 0; i < m; i++){
    readCnt ++;
    int a, b;
    double p;
    int c = fscanf(fin, "%d%d%lf", &a, &b, &p);
    ASSERTT(c == 3, a, b, p, c);

    //TRACE_LINE(a, b);
    // check if node ids within graph array range
    ASSERT( a < n );
    ASSERT( b < n );
    hasnode[a] = true;
    hasnode[b] = true;
    addEdge(a, b, p);
  }
  //TRACE_LINE_END();
  int s = 0;
  for (int i = 0; i < n; i++)
      if (hasnode[i])
        s++;
  INFO(s);
  ASSERT(readCnt == m);
  fclose(fin);
}

void Graph::readGraphBin(){
    string graph_file_bin = graph_file.substr(0, graph_file.size() - 3) + "bin";
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
  cout << "- nodes      : " << toColor("yellow", to_string(n)) << endl;
  cout << "- edges      : " << toColor("yellow", to_string(m)) << endl;
  cout << string(50, '-') << endl;
}

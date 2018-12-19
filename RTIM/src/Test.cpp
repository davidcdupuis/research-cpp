#include <string>
#include <iostream>
#include <map>
#include <fstream>


using namespace std;

class Graph{
public:
  string dataset;
  Graph();
};

class Algorithm{
public:
  std::map<std::string, std::map<std::string, std::string> > datasets;
  Graph& graph;
  Algorithm(Graph& g);

  void loadDatasets();
};

class Main{
public:
  Graph& graph;
  Algorithm& algo;
  Main(Graph& g, Algorithm& a);
};

Graph::Graph(){}

Algorithm::Algorithm(Graph& g):graph(g){
  // graph = g;
}

void Algorithm::loadDatasets(){
  string path = "../../data/datasets.txt";
  string name, id, nodes, edges;
  ifstream infile(path.c_str());
  while(infile >> name >> id >> nodes >> edges){
    map<string, string> tmpMap;
    tmpMap.insert(pair<string, string>("id", id));
    tmpMap.insert(pair<string, string>("nodes", nodes));
    tmpMap.insert(pair<string, string>("edges", edges));
    datasets.insert(pair<string, map<string, string> >(name, tmpMap));
    // datasetNames.push_back(name);
    // datasetIds.push_back(id);
    // datasetNodes.push_back(nodes);
    // datasetEdges.push_back(edges);
  }
  for(auto it = datasets.begin(); it != datasets.end(); ++it){
    cout << it->first << " : " << endl;
    cout << "\t - id : " << it->second["id"] << endl;
    cout << "\t - nodes : " << it->second["nodes"] << endl;
    cout << "\t - edges : " << it->second["edges"] << endl;
  }
  // cout << "nethept: " << endl;
  // cout << " - id: "<< datasets["nethept"]["id"] << endl;
  // cout << " - nodes: " << datasets["nethept"]["nodes"] << endl;
  // cout << " - nodes: " << datasets["nethept"]["edges"] << endl;
}

Main::Main(Graph& g, Algorithm& a):graph(g),algo(a){
  // graph = g;
}

int main(){
  Graph graph = Graph();
  graph.dataset = "nethept";
  Algorithm algo = Algorithm(graph);
  algo.loadDatasets();
}
